/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 14:32:09 by tmarcos           #+#    #+#             */
/*   Updated: 2026/02/12 12:41:28 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/core/ServerManager.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <iostream>

ServerManager::ServerManager() {}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < servers.size(); ++i)
		delete servers[i];
	servers.clear();
}

// Validates that no two servers are configured to listen on the same port
void ServerManager::validatePorts(std::vector<ServerConfig>& configs) {
	std::set<int> usedPorts;
	// Check each config for duplicate ports
	for (size_t i = 0; i < configs.size(); ++i) {
		int port = configs[i].getPort();
		if (usedPorts.find(port) != usedPorts.end()) {
			std::cerr << "ERROR: Port " << port << " duplicated in config!" << std::endl;
			exit(1);
		} 
		usedPorts.insert(port);
	}
	std::cout << "[ServerManager] Port validation: OK (" << configs.size() << " servers)" << std::endl;
}

// Initializes all servers from config and sets up listen sockets
void ServerManager::initialize(std::vector<ServerConfig>& configs) {
	std::cout << "[ServerManager] Initializing " << configs.size() << " server(s)..." << std::endl;
	validatePorts(configs); // Verify no duplicate ports
	// Create and setup each server
	for (size_t i = 0; i < configs.size(); ++i) {
		ServerConfig& cfg = configs[i];
		int port = cfg.getPort();
		Server* server = new Server(port, &cfg); // Create new server instance with config
		server->setupListenSocket();
		// Map listening socket to server for event dispatch
		int listenFd = server->getListenFd();
		listenFdToServer[listenFd] = server;
		servers.push_back(server);
		std::cout << "[ServerManager] Server initialized: port=" << port 
		          << " listenFd=" << listenFd << std::endl;
	}
	std::cout << "[ServerManager] All servers ready!" << std::endl;
}

// Builds poll array with all listening sockets and client sockets (state-aware)
void ServerManager::buildPollArray(std::vector<struct pollfd>& fds) {
	fds.clear();
	// Add all listening sockets (for accepting new connections)
	for (size_t i = 0; i < servers.size(); ++i) {
		struct pollfd pfd;
		pfd.fd = servers[i]->getListenFd();
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
	}
	// Add all client sockets with state-aware event flags
	for (size_t i = 0; i < servers.size(); ++i) {
		std::map<int, Client>& clients = servers[i]->getClients();
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
			struct pollfd pfd;
			pfd.fd = it->first;
			short events = 0;
			ClientState st = it->second.getState();
			// Request POLLIN if client is reading, idle, or just accepted
			if (st == READING || st == IDLE || st == ACCEPTED)
				events |= POLLIN;
			// Request POLLOUT if client is writing or has pending writes
			if (st == WRITING || it->second.hasWrite())
				events |= POLLOUT;
			pfd.events = events;
			pfd.revents = 0;
			fds.push_back(pfd);
		}
	}
}

// Updates the client-to-server mapping for a given server
void ServerManager::updateClientMapping(Server* server) {
	std::map<int, Client>& clients = server->getClients();
	
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		int clientFd = it->first;
		if (clientFdToServer.find(clientFd) == clientFdToServer.end()) {
			clientFdToServer[clientFd] = server;
		}
	}
}

// Handles incoming connections on a listening socket
void ServerManager::handleListenSocket(int fd) {
	std::map<int, Server*>::iterator it = listenFdToServer.find(fd);
	
	if (it == listenFdToServer.end()) {
		std::cerr << "[ServerManager] ERROR: listenFd " << fd << " not found!" << std::endl;
		return;
	}
	Server* server = it->second;
	server->acceptNewClients();
	updateClientMapping(server);
}

// Handles read/write events for a client socket
void ServerManager::handleClientSocket(int fd, short revents) {
	// Find the server managing this client
	std::map<int, Server*>::iterator serverIt = clientFdToServer.find(fd);
	if (serverIt == clientFdToServer.end()) {
		std::cerr << "[ServerManager] ERROR: clientFd " << fd << " not found!" << std::endl;
		return;
	}
	Server* server = serverIt->second;
	std::map<int, Client>& clients = server->getClients();
	// Double-check the client still exists
	std::map<int, Client>::iterator clientIt = clients.find(fd);
	if (clientIt == clients.end()) {
		clientFdToServer.erase(serverIt);
		return;
	} if (revents & POLLIN) // Handle read event if socket is readable
		server->handleClientRead(fd, clientIt);
	if (clients.find(fd) == clients.end()) { // Check if client was closed during read
		clientFdToServer.erase(fd);
		return;
	} if (revents & POLLOUT) // Handle write event if socket is writable
		server->handleClientWrite(fd, clientIt);
	if (clients.find(fd) == clients.end()) { // Check if response is complete and should close
		clientFdToServer.erase(fd);
		return;
	} server->closeIfComplete(fd, clientIt);
	if (clients.find(fd) == clients.end())
		clientFdToServer.erase(fd);
}

// Main event loop - polls all sockets and dispatches events
void ServerManager::run() {
	std::cout << "[ServerManager] Starting main loop..." << std::endl;
	std::cout << "[ServerManager] Listening on " << servers.size() << " port(s):" << std::endl;
	for (size_t i = 0; i < servers.size(); ++i)
		std::cout << "  - Port " << servers[i]->getPort() 
		          << " (fd=" << servers[i]->getListenFd() << ")" << std::endl;
	while (true) {
		// Rebuild poll array with current state of all sockets
		std::vector<struct pollfd> pollFds;
		buildPollArray(pollFds);
		// Wait up to 1000ms for events on any socket
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		// Handle poll errors
		if (ready < 0) {
			if (errno == EINTR)
				continue;  // Interrupted by signal, retry
			std::cerr << "[ServerManager] poll() error: " << strerror(errno) << std::endl;
			break;
		} if (ready == 0) {
			// No events -- still check timeouts on idle tick
		}
		// Timeout sweep (check every iteration, including idle ticks)
		time_t now = std::time(NULL);
		for (size_t i = 0; i < servers.size(); ++i)
			servers[i]->checkTimeouts(now, 60);
		// Process all sockets with events
		for (size_t i = 0; i < pollFds.size(); ++i) {
			if (pollFds[i].revents == 0)
				continue;  // No events on this socket
			int fd = pollFds[i].fd;
			short revents = pollFds[i].revents;
			// Handle socket errors first (disconnection, errors)
			if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
				if (clientFdToServer.find(fd) != clientFdToServer.end()) {
					Server* server = clientFdToServer[fd];
					std::map<int, Client>& clients = server->getClients();
					std::map<int, Client>::iterator it = clients.find(fd);
					if (it != clients.end()) {
						close(fd);
						clients.erase(it);
					}
					clientFdToServer.erase(fd);
				}
				continue;
			}
			// Check if this is a listening socket (new connection)
			if (listenFdToServer.find(fd) != listenFdToServer.end()) {
				if (revents & POLLIN)
					handleListenSocket(fd);
			}
			// Or a client socket (read/write)
			else if (clientFdToServer.find(fd) != clientFdToServer.end())
				handleClientSocket(fd, revents);
		}
	}
}
