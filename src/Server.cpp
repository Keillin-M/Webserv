/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/03 11:44:46 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

Server::Server(int port) : listenFd(-1), port(port) {}

void Server::run() {
	listenFd = socket(AF_INET, SOCK_STREAM, 0); //ipv4, TCP, default

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr)); //iniciate memory
	addr.sin_family = AF_INET;  //ipv4
	addr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY = 0.0.0.0.
	addr.sin_port = htons(static_cast<unsigned short>(port)); //converts the port number to host

	bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
	listen(listenFd, SOMAXCONN);

	// (subject) "non-blocking at all times"
	int flags = fcntl(listenFd, F_GETFL, 0);
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);

	while (true) {
		// STEP 1: Rebuild poll array each iteration (option B)
		// Index 0 = listen fd, index 1..N = connected clients
		std::vector<struct pollfd> pollFds; //creates a empty vector to store the fds
		struct pollfd pfd;

		pfd.fd = listenFd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		pollFds.push_back(pfd);

		// add each client fd from the map
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
			pfd.fd = it->first;
			// request read events, and ask for POLLOUT only if there's pending data
			pfd.events = POLLIN | (it->second.hasWrite() ? POLLOUT : 0);
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}

		// STEP 2: Wait until some fd has activity or timeout
		// (subject) "never hang indefinitely" - timeout 1000ms
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			break;
		}

		// STEP 3: POLLIN on listen fd = new connection waiting
		if (pollFds[0].revents & POLLIN) {
			int clientFd = accept(listenFd, NULL, NULL);
			if (clientFd >= 0) {
				// (subject) "non-blocking at all times"
				int clientFlags = fcntl(clientFd, F_GETFL, 0);
				fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
				clients.insert(std::make_pair(clientFd, Client(clientFd)));
			}
		}
		// STEP 4: process client events; pollFds[0] is listen fd, remaining entries map to clients
		for (size_t idx = 1; idx < pollFds.size(); ++idx) {
			struct pollfd &entry = pollFds[idx];
			int cfd = entry.fd;

			std::map<int, Client>::iterator it = clients.find(cfd);
			if (it == clients.end())
				continue; // client might have been removed

			// HANDLE READ
			if (entry.revents & POLLIN) {
				char buf[4096];
				ssize_t bytesRead = recv(cfd, buf, sizeof(buf), 0);

				if (bytesRead > 0) {
					// store in client's read buffer
					it->second.appendRead(buf, static_cast<size_t>(bytesRead));
					// TODO: parse HTTP request from it->second.read_buffer() and prepare response
				} else if (bytesRead == 0) {
					// peer closed connection
					close(cfd);
					clients.erase(it);
					continue;
				} else {
					// bytesRead < 0: non-blocking socket has no data available or error occurred
					// continue to next iteration without closing connection
				}
			}

			// HANDLE WRITE
			if (entry.revents & POLLOUT) {
				std::string &out = it->second.getWriteBuffer();
				while (!out.empty()) {
					ssize_t sent = send(cfd, out.data(), out.size(), 0);
					if (sent > 0) {
						out.erase(0, static_cast<size_t>(sent));
						continue;
					} if (sent < 0) {
						break; // send failed, stop trying for now
					}
					// sent == 0, nothing sent, continue
				}

				// if all data sent, close unless keep-alive is set
				if (it != clients.end()) {
					if (it->second.getWriteBuffer().empty()) {
						if (!it->second.isKeepAlive()) {
							close(cfd);
							clients.erase(it);
						}
					}
				}
			}
		}
	}
}
