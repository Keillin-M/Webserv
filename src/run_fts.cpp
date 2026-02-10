/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_fts.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:24:17 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/10 12:40:02 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

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
	if (bind(listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cerr << "bind error on port " << port << ": " << strerror(errno) << std::endl;
		close(listenFd);
		listenFd = -1;
		return;
	}
	if (listen(listenFd, SOMAXCONN) < 0) {
		std::cerr << "listen error on port " << port << ": " << strerror(errno) << std::endl;
		close(listenFd);
		listenFd = -1;
		return;
	}

	// (subject) "non-blocking at all times"
	int flags = fcntl(listenFd, F_GETFL, 0);
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);
}

void Server::createPollFds(std::vector<struct pollfd>& pollFds, struct pollfd pfd) {
	pfd.fd = listenFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	// add each client fd — state-aware event registration
	std::map<int, Client>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		pfd.fd = it->first;
		short events = 0;
		ClientState st = it->second.getState();
		if (st == READING || st == IDLE || st == ACCEPTED)
			events |= POLLIN;
		if (st == WRITING || it->second.hasWrite())
			events |= POLLOUT;
		pfd.events = events;
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
		clients.find(clientFd)->second.setState(READING);
		std::cout << "[Server] Client connected: fd=" << clientFd << std::endl;
	}
}

void Server::handleClientWrite(int cfd, std::map<int, Client>::iterator it) {
	std::string &out = it->second.getWriteBuffer();
	while (!out.empty()) {
		ssize_t sent = send(cfd, out.data(), out.size(), 0);
		if (sent > 0) {
			out.erase(0, static_cast<size_t>(sent));
			continue;
		} else
			break;
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