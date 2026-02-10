/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_fts.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:24:17 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/10 15:03:09 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

// Initialize socket address structure for IPv4
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
	// Allow socket reuse (avoid TIME_WAIT binding issues)
	int opt = 1;
	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	iniciateAddr(addr, port);
	// Bind socket to specified port
	if (bind(listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cerr << "bind error on port " << port << ": " << strerror(errno) << std::endl;
		close(listenFd);
		listenFd = -1;
		return;
	} if (listen(listenFd, SOMAXCONN) < 0) { // Start listening for connections
		std::cerr << "listen error on port " << port << ": " << strerror(errno) << std::endl;
		close(listenFd);
		listenFd = -1;
		return;
	} int flags = fcntl(listenFd, F_GETFL, 0); // Set non-blocking mode (required by subject)
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);
}

// Build poll array: add listening socket + all client sockets with state-aware events
void Server::createPollFds(std::vector<struct pollfd>& pollFds, struct pollfd pfd) {
	// Add listening socket (always waiting for new connections)
	pfd.fd = listenFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	// Add all client sockets with appropriate event flags
	std::map<int, Client>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		pfd.fd = it->first;
		short events = 0;
		ClientState st = it->second.getState();
		// Request POLLIN if reading, idle, or just accepted
		if (st == READING || st == IDLE || st == ACCEPTED)
			events |= POLLIN;
		// Request POLLOUT if writing or has pending data
		if (st == WRITING || it->second.hasWrite())
			events |= POLLOUT;
		pfd.events = events;
		pfd.revents = 0;
		pollFds.push_back(pfd);
	}
}

// Accept all waiting connections and set them to non-blocking
void Server::acceptNewClients() {
	while (true) {
		// Try to accept next connection
		int clientFd = accept(listenFd, NULL, NULL);
		if (clientFd > 0) {
			// Set non-blocking mode (required by subject)
			int clientFlags = fcntl(clientFd, F_GETFL, 0);
			fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
			// Add client to map and initialize with READING state
			clients.insert(std::make_pair(clientFd, Client(clientFd)));
			clients.find(clientFd)->second.setState(READING);
			std::cout << "[Server] Client connected: fd=" << clientFd << std::endl;
		} else
			break;  // No more connections waiting
	}
}

// Send data from write buffer to client socket (single send per POLLOUT)
void Server::handleClientWrite(int cfd, std::map<int, Client>::iterator it) {
	std::string &out = it->second.getWriteBuffer();
	if (out.empty())
		return;
	ssize_t sent = send(cfd, out.data(), out.size(), 0);
	if (sent > 0) {
		out.erase(0, static_cast<size_t>(sent));
		it->second.updateLastSeen();
	} else {
		// sent <= 0: error -- close client (no errno check per subject)
		it->second.setState(CLOSING);
		close(cfd);
		clients.erase(it);
	}
}
			
// Check if response is complete and transition to IDLE or close
void Server::closeIfComplete(int cfd, std::map<int, Client>::iterator it) {
	if (!it->second.getWriteBuffer().empty())
		return;
	if (it->second.isKeepAlive()) {
		it->second.setState(IDLE);
		it->second.clearReadBuffer();
	} else {
		it->second.setState(CLOSING);
		close(cfd);
		clients.erase(it);
	}
}

// Close idle clients that exceed timeout (two-pass to avoid iterator invalidation)
void Server::checkTimeouts(time_t now, int timeoutSecs) {
	std::vector<int> toClose;
	for (std::map<int, Client>::iterator it = clients.begin();
		 it != clients.end(); ++it) {
		if (now - it->second.getLastSeen() > timeoutSecs)
			toClose.push_back(it->first);
	}
	for (size_t i = 0; i < toClose.size(); ++i) {
		std::map<int, Client>::iterator it = clients.find(toClose[i]);
		if (it != clients.end()) {
			it->second.setState(CLOSING);
			close(it->first);
			clients.erase(it);
		}
	}
}