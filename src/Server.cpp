/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 16:04:49 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(int port) : listenFd(-1), port(port) {(void)port;}

void Server::run() {
	// Create socket and bind
	listenFd = socket(AF_INET, SOCK_STREAM, 0); //IPv4, TCP, default protocol
	bind(listenFd, (struct sockaddr *)&address, sizeof(address));
	listen(listenFd, SOMAXCONN);
	
	// Fix non-blocking
	int flags = fcntl(listenFd, F_GETFL, 0); // get file status flags
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);

	while (true) {
		int clientFd = accept(listenFd, NULL, NULL);
		if (clientFd >= 0) {
			int clientFlags = fcntl(clientFd, F_GETFL, 0);
			fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
		}
	}
}
