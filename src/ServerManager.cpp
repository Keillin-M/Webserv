/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmarcos <tmarcos@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 14:32:09 by tmarcos           #+#    #+#             */
/*   Updated: 2026/02/09 14:50:00 by tmarcos          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <iostream>

ServerManager::ServerManager() {}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < servers.size(); ++i) {
		delete servers[i];
	}
	servers.clear();
}

void ServerManager::validatePorts(std::vector<ServerConfig>& configs) {
	std::set<int> usedPorts;
	
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

void ServerManager::initialize(std::vector<ServerConfig>& configs) {
	std::cout << "[ServerManager] Initializing " << configs.size() << " server(s)..." << std::endl;
	
	validatePorts(configs);
	
	for (size_t i = 0; i < configs.size(); ++i) {
		ServerConfig& cfg = configs[i];
		int port = cfg.getPort();
		
		Server* server = new Server(port, &cfg);
		server->setupListenSocket();
		
		int listenFd = server->getListenFd();
		listenFdToServer[listenFd] = server;
		servers.push_back(server);
		
		std::cout << "[ServerManager] Server initialized: port=" << port 
		          << " listenFd=" << listenFd << std::endl;
	}
	std::cout << "[ServerManager] All servers ready!" << std::endl;
}

void ServerManager::buildPollArray(std::vector<struct pollfd>& fds) {
	fds.clear();
	
	// Add all listening sockets
	for (size_t i = 0; i < servers.size(); ++i) {
		struct pollfd pfd;
		pfd.fd = servers[i]->getListenFd();
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
	}
	
	// Add all client sockets — state-aware event registration
	for (size_t i = 0; i < servers.size(); ++i) {
		std::map<int, Client>& clients = servers[i]->getClients();
		
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
			struct pollfd pfd;
			pfd.fd = it->first;
			short events = 0;
			ClientState st = it->second.getState();
			if (st == READING || st == IDLE || st == ACCEPTED)
				events |= POLLIN;
			if (st == WRITING || it->second.hasWrite())
				events |= POLLOUT;
			pfd.events = events;
			pfd.revents = 0;
			fds.push_back(pfd);
		}
	}
}

void ServerManager::updateClientMapping(Server* server) {
	std::map<int, Client>& clients = server->getClients();
	
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		int clientFd = it->first;
		
		if (clientFdToServer.find(clientFd) == clientFdToServer.end()) {
			clientFdToServer[clientFd] = server;
		}
	}
}

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

void ServerManager::handleClientSocket(int fd, short revents) {
	std::map<int, Server*>::iterator serverIt = clientFdToServer.find(fd);
	
	if (serverIt == clientFdToServer.end()) {
		std::cerr << "[ServerManager] ERROR: clientFd " << fd << " not found!" << std::endl;
		return;
	}
	
	Server* server = serverIt->second;
	std::map<int, Client>& clients = server->getClients();
	std::map<int, Client>::iterator clientIt = clients.find(fd);
	
	if (clientIt == clients.end()) {
		clientFdToServer.erase(serverIt);
		return;
	}
	
	// Handle read
	if (revents & POLLIN) {
		server->handleClientRead(fd, clientIt);
	}
	
	if (clients.find(fd) == clients.end()) {
		clientFdToServer.erase(fd);
		return;
	}
	
	// Handle write
	if (revents & POLLOUT) {
		server->handleClientWrite(fd, clientIt);
	}
	
	if (clients.find(fd) == clients.end()) {
		clientFdToServer.erase(fd);
		return;
	}
	
	// Close if complete
	server->closeIfComplete(fd, clientIt);
	
	if (clients.find(fd) == clients.end()) {
		clientFdToServer.erase(fd);
	}
}

void ServerManager::run() {
	std::cout << "[ServerManager] Starting main loop..." << std::endl;
	std::cout << "[ServerManager] Listening on " << servers.size() << " port(s):" << std::endl;
	
	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << "  - Port " << servers[i]->getPort() 
		          << " (fd=" << servers[i]->getListenFd() << ")" << std::endl;
	}
	
	while (true) {
		std::vector<struct pollfd> pollFds;
		buildPollArray(pollFds);
		
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			std::cerr << "[ServerManager] poll() error: " << strerror(errno) << std::endl;
			break;
		}
		
		if (ready == 0)
			continue;
		
		// Process events
		for (size_t i = 0; i < pollFds.size(); ++i) {
			if (pollFds[i].revents == 0)
				continue;
			
			int fd = pollFds[i].fd;
			short revents = pollFds[i].revents;
			
			// Listen socket?
			if (listenFdToServer.find(fd) != listenFdToServer.end()) {
				if (revents & POLLIN) {
					handleListenSocket(fd);
				}
			}
			// Client socket?
			else if (clientFdToServer.find(fd) != clientFdToServer.end()) {
				handleClientSocket(fd, revents);
			}
		}
	}
}
