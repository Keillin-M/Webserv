/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/04 18:25:10 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>

Server::Server(int port) : listenFd(-1), port(port) {}

const std::map<int, Client>& Server::getClients() const {
	return clients;
}

std::map<int, Client>& Server::getClients() {
	return clients;
}

void Server::setListenFd(int newFd) {
	listenFd = newFd;
}

int Server::getListenFd() const {
	return listenFd;
}

static void iniciate_addr(struct sockaddr_in& addr, int port) {
	std::memset(&addr, 0, sizeof(addr)); //iniciate memory
	addr.sin_family = AF_INET;  //ipv4
	addr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY = 0.0.0.0.
	addr.sin_port = htons(static_cast<unsigned short>(port)); //converts the port number to host
}

void setup_listen_socket(int &listenFd, int port) {
	struct sockaddr_in addr;
	
	listenFd = socket(AF_INET, SOCK_STREAM, 0); //ipv4, TCP, default
	if (listenFd < 0) {
		std::cerr << "socket error: " << strerror(errno) << std::endl;
		return;
	}
	
	// Allow immediate restart
	int opt = 1;
	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	iniciate_addr(addr, port);
	bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
	listen(listenFd, SOMAXCONN);

	// (subject) "non-blocking at all times"
	int flags = fcntl(listenFd, F_GETFL, 0);
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);
}

void create_clients(std::vector<struct pollfd>& pollFds, struct pollfd pfd, Server& serv) {
	pfd.fd = serv.getListenFd();
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	// add each client fd from the map
	std::map<int, Client>::const_iterator it;
	for (it = serv.getClients().begin(); it != serv.getClients().end(); ++it) {
		pfd.fd = it->first;
		// request read events, and ask for POLLOUT only if there's pending data
		pfd.events = POLLIN | (it->second.hasWrite() ? POLLOUT : 0);
		pfd.revents = 0;
		pollFds.push_back(pfd);
	}
}

void pollin_clients(Server& serv) {
	while (true) {
		int clientFd = accept(serv.getListenFd(), NULL, NULL);
		if (clientFd < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break; // no more connections waiting
			std::cerr << "accept error: " << strerror(errno) << std::endl;
			break;
		}
		// (subject) "non-blocking at all times"
		int clientFlags = fcntl(clientFd, F_GETFL, 0);
		fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
		serv.getClients().insert(std::make_pair(clientFd, Client(clientFd)));
		std::cout << "[Server] Client connected: fd=" << clientFd << std::endl;
	}
}

static std::string buildResponse(int statusCode) {
	std::string body;
	std::string statusText;
	
	if (statusCode == 200) {
		statusText = "OK";
		body = "Hello, world!";
	} else if (statusCode == 404) {
		statusText = "Not Found";
		body = "404 Not found";
	} else {
		statusText = "Internal Server Error";
		body = "500 Internal Server Error";
	}
	
	std::ostringstream oss;
	oss << statusCode;
	std::string statusCodeStr = oss.str();
	
	std::ostringstream oss2;
	oss2 << body.size();
	std::string contentLength = oss2.str();
	
	std::string response;
	response += "HTTP/1.1 " + statusCodeStr + " " + statusText + "\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: " + contentLength + "\r\n";
	response += "\r\n"; //End of header
	response += body;
	
	return response;
}

void gnl_clients(int cfd, std::map<int, Client>::iterator it, Server& serv){
	char buf[4096];
	ssize_t bytesRead = recv(cfd, buf, sizeof(buf), 0);
	if (bytesRead > 0) {
		// store in client's read buffer
		it->second.appendRead(buf, static_cast<size_t>(bytesRead));
		// TODO: parse HTTP request from it->second.read_buffer() and prepare response
		if (it->second.requestCompleteCheck()) {
			Request request;
			request.parse(it->second.getReadBuffer()); // Gab parsing
			
			if (request.getMethod() == "GET" && request.getPath() == "/")
				it->second.appendWrite(buildResponse(200));
			else
				it->second.appendWrite(buildResponse(404));
			it->second.clearReadBuffer();
		}
	} else if (bytesRead == 0) {
		// peer closed connection
		close(cfd);
		serv.getClients().erase(it);
	} else {
		// < 0: non-blocking, no data available - skip for now
	}
}

void handle_write(int cfd, std::map<int, Client>::iterator it) {
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
			
void closed_empty_fds(int cfd, std::map<int, Client>::iterator it, Server& serv){
	if (it->second.getWriteBuffer().empty()) {
		if (!it->second.isKeepAlive()) {
			close(cfd);
			serv.getClients().erase(it);
		}
	}
}			

void Server::run() {
	setup_listen_socket(listenFd, port);

	while (true) {
		// STEP 1: Rebuild poll array each iteration (option B)
		// Index 0 = listen fd, index 1..N = connected clients
		std::vector<struct pollfd> pollFds; //creates a empty vector to store the fds
		struct pollfd pfd;

		create_clients(pollFds, pfd, *this);

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
			pollin_clients(*this);		
		// STEP 4: process client events; pollFds[0] is listen fd, remaining entries map to clients
		for (size_t idx = 1; idx < pollFds.size(); ++idx) {
			struct pollfd &entry = pollFds[idx];
			int cfd = entry.fd;

			std::map<int, Client>::iterator it = clients.find(cfd);
			if (it == clients.end())
				continue; // client might have been removed

			// HANDLE READ
			if (entry.revents & POLLIN)
				gnl_clients(cfd, it, *this);

			// HANDLE WRITE
			if (entry.revents & POLLOUT) 
				handle_write(cfd, it);

				// if all data sent, close unless keep-alive is set
			if (it != clients.end()) 
				closed_empty_fds(cfd, it, *this);
		}
	}
}
