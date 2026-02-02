/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmarcos <tmarcos@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 18:07:25 by tmarcos          ###   ########.fr       */
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
	listenFd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(static_cast<unsigned short>(port));

	bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
	listen(listenFd, SOMAXCONN);

	// (subject) "non-blocking at all times"
	int flags = fcntl(listenFd, F_GETFL, 0);
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);

	while (true) {
		// STEP 1: Rebuild poll array each iteration (option B)
		// Index 0 = listen fd, index 1..N = connected clients
		std::vector<struct pollfd> pollFds; //cria um vetor vazio pra guardar os fds
		struct pollfd pfd;

		pfd.fd = listenFd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		pollFds.push_back(pfd);

		for (size_t i = 0; i < clients.size(); i++) {
			pfd.fd = clients[i];
			pfd.events = POLLIN;
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
				clients.push_back(clientFd);
			}
		}

		// STEP 4: POLLIN on client fd = data available to read
		for (size_t i = 0; i < clients.size(); i++) {
			if (!(pollFds[i + 1].revents & POLLIN))
				continue;

			char buf[4096];
			ssize_t bytesRead = recv(clients[i], buf, sizeof(buf), 0);

			if (bytesRead > 0) {
				// TODO: store in client buffer, parse HTTP request
				(void)buf;
			} else {
				// Client disconnected or error - close and remove
				close(clients[i]); //close socket
				clients[i] = clients.back();
				clients.pop_back(); //remove from vector
				i--;
			}
		}
	}
}
