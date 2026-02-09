/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 18:07:14 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include "../include/Response.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Construtor original (para compatibilidade)
Server::Server(int port) : listenFd(-1), port(port), config(NULL) {}

// Novo construtor para ServerManager (recebe config)
Server::Server(int port, ServerConfig* cfg) : listenFd(-1), port(port), config(cfg) {}

std::map<int, Client>& Server::getClients() {
	return clients;
}

void Server::setListenFd(int newFd) {
	listenFd = newFd;
}

int Server::getListenFd() const {
	return listenFd;
}

int Server::getPort() const {
	return port;
}

ServerConfig* Server::getConfig() {
	return config;
}

static void iniciateAddr(struct sockaddr_in& addr, int port) {
	std::memset(&addr, 0, sizeof(addr)); //iniciate memory
	addr.sin_family = AF_INET;  //ipv4
	addr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY = 0.0.0.0.
	addr.sin_port = htons(static_cast<unsigned short>(port)); //converts the port number to host
}

void Server::setupListenSocket() {
	struct sockaddr_in addr;
	
	listenFd = socket(AF_INET, SOCK_STREAM, 0); //ipv4, TCP, default
	if (listenFd < 0) {
		std::cerr << "socket error: " << strerror(errno) << std::endl;
		return;
	}
	
	// Allow immediate restart
	int opt = 1;
	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	iniciateAddr(addr, port);
	bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
	listen(listenFd, SOMAXCONN);

	// (subject) "non-blocking at all times"
	int flags = fcntl(listenFd, F_GETFL, 0);
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);
}

void Server::createPollFds(std::vector<struct pollfd>& pollFds, struct pollfd pfd) {
	pfd.fd = listenFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	// add each client fd from the map
	std::map<int, Client>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		pfd.fd = it->first;
		// request read events, and ask for POLLOUT only if there's pending data
		pfd.events = POLLIN | (it->second.hasWrite() ? POLLOUT : 0);
		pfd.revents = 0;
		pollFds.push_back(pfd);
	}
}

void Server::acceptNewClients() {
	while (true) {
		int clientFd = accept(listenFd, NULL, NULL);
		if (clientFd < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break; // no more connections waiting
			std::cerr << "accept error: " << strerror(errno) << std::endl;
			break;
		}
		// (subject) "non-blocking at all times"
		int clientFlags = fcntl(clientFd, F_GETFL, 0);
		fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
		clients.insert(std::make_pair(clientFd, Client(clientFd)));
		std::cout << "[Server] Client connected: fd=" << clientFd << std::endl;
	}
}

void Server::handleClientRead(int cfd, std::map<int, Client>::iterator it){
	char buf[4096];
	ssize_t bytesRead = recv(cfd, buf, sizeof(buf), 0);
	if (bytesRead > 0) {
		it->second.appendRead(buf, static_cast<size_t>(bytesRead));
		if (it->second.requestCompleteCheck()) {
			Request request;
			request.parseRequest(it->second.getReadBuffer());
			
			Response response;
			std::string httpResponse;

			// Validate HTTP version - only accept HTTP/1.1
			if (request.getVersion() != "HTTP/1.1") {
				httpResponse = response.errorResponse(505, "HTTP Version Not Supported");
				it->second.appendWrite(httpResponse);
				it->second.clearReadBuffer();
				return;
			}
			
			const LocationConfig* matchedLocation = config->findMatchLocation(request.getPath());
			
			if (matchedLocation == NULL) {
				std::string errorPagePath;
				std::map<int, std::string> errorPages = config->getErrorPages();
				if (errorPages.find(500) != errorPages.end()) {
				errorPagePath = config->getRoot() + "/" + errorPages[500];
				}
				httpResponse = response.errorResponse(500, "Internal Server Error", errorPagePath);
				it->second.appendWrite(httpResponse);
				it->second.clearReadBuffer();
				return ;
			}

			std::string rootDir = matchedLocation->getRoot();
			
			if (!matchedLocation->isMethodAllowed(request.getMethod())) {
				std::string errorPagePath;
				std::map<int, std::string> errorPages = config->getErrorPages();
				if (errorPages.find(405) != errorPages.end()) {
					errorPagePath = rootDir + "/" + errorPages[405];
				}
				httpResponse = response.errorResponse(405,"Method Not Allowed", errorPagePath);
				it->second.appendWrite(httpResponse);
				it->second.clearReadBuffer();
				return ;
			}
			
			// Resolve root: use location root, fallback to server root
			if (rootDir.empty()) {
				rootDir = config->getRoot();
			}
			
			// Get index file from location (or default)
			std::string indexFile = matchedLocation->getIndexFile();
			if (indexFile.empty()) {
				indexFile = "index.html";
			}
			
			std::string uploadDir = rootDir + "/uploads";
			
			if (request.getMethod() == "GET") {
				httpResponse = response.handleGet(request.getPath(), rootDir, indexFile);
			} else if (request.getMethod() == "POST") {
				httpResponse = response.handlePost(request.getBody(), uploadDir);
			} else if (request.getMethod() == "DELETE") {
				// Check if path starts with /upload to determine directory
				std::string path = request.getPath();
				std::string deleteDir = (path.find("upload") != std::string::npos) ? uploadDir : rootDir;
				httpResponse = response.handleDelete(path, deleteDir);
			} else
				httpResponse = response.errorResponse(501, "Method not allowed");
			it->second.appendWrite(httpResponse);
			it->second.clearReadBuffer();
		}
	} else if (bytesRead == 0) {
		close(cfd);
		clients.erase(it);
	} else {
		// < 0: non-blocking, no data available - skip for now
	}
}

void Server::handleClientWrite(int cfd, std::map<int, Client>::iterator it) {
	std::string &out = it->second.getWriteBuffer();
	while (!out.empty()) {
		ssize_t sent = send(cfd, out.data(), out.size(), 0);
		if (sent > 0) {
			out.erase(0, static_cast<size_t>(sent));
			continue;
		} if (sent <= 0)
			break; // send failed, stop trying for now
		// sent == 0, nothing sent, continue
	}
}
			
void Server::closeIfComplete(int cfd, std::map<int, Client>::iterator it){
	if (it->second.getWriteBuffer().empty()) {
		if (!it->second.isKeepAlive()) {
			close(cfd);
			clients.erase(it);
		}
	}
}			

void Server::run() {
	setupListenSocket();

	while (true) {
		// STEP 1: Rebuild poll array each iteration (option B)
		// Index 0 = listen fd, index 1..N = connected clients
		std::vector<struct pollfd> pollFds; //creates a empty vector to store the fds
		struct pollfd pfd;

		createPollFds(pollFds, pfd);

		// STEP 2: Wait until some fd has activity or timeout
		// (subject) "never hang indefinitely" - timeout 1000ms
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			break;
		}
		// STEP 3: POLLIN on listen fd = new connection waiting
		if (pollFds[0].revents & POLLIN)
			acceptNewClients();		
		// STEP 4: process client events; pollFds[0] is listen fd, remaining entries map to clients
		for (size_t idx = 1; idx < pollFds.size(); ++idx) {
			struct pollfd &entry = pollFds[idx];
			int cfd = entry.fd;

			std::map<int, Client>::iterator it = clients.find(cfd);
			if (it == clients.end())
				continue; // client might have been removed

			// HANDLE READ
			if (entry.revents & POLLIN)
				handleClientRead(cfd, it);

			// HANDLE WRITE
			if (entry.revents & POLLOUT) 
				handleClientWrite(cfd, it);

				// if all data sent, close unless keep-alive is set
			if (it != clients.end()) 
				closeIfComplete(cfd, it);
		}
	}
}
