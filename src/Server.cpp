/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/10 12:40:06 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

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

void Server::run() {
	setupListenSocket();
	while (true) {
		std::vector<struct pollfd> pollFds;
		struct pollfd pfd;
		createPollFds(pollFds, pfd);
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			break;
		}
		// Timeout sweep (check every iteration, including idle ticks)
		time_t now = std::time(NULL);
		checkTimeouts(now, 60);
		if (pollFds[0].revents & POLLIN)
			acceptNewClients();		
		for (size_t idx = 1; idx < pollFds.size(); ++idx) {
			struct pollfd &entry = pollFds[idx];
			int cfd = entry.fd;
			std::map<int, Client>::iterator it = clients.find(cfd);
			if (it == clients.end())
				continue;
			if (entry.revents & POLLIN)
				handleClientRead(cfd, it);
			if (entry.revents & POLLOUT) 
				handleClientWrite(cfd, it);
			if (it != clients.end()) 
				closeIfComplete(cfd, it);
		}
	}
}