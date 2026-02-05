/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:56:57 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/05 15:46:59 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <map>
#include "Client.hpp"

class Server {
	private:
		int listenFd; //listens the socket fd
		int port;
		std::map<int, Client> clients;

		void setupListenSocket();
		void createPollFds(std::vector<struct pollfd>& pollFds, struct pollfd pfd);
		void acceptNewClients();
		void handleClientRead(int cfd, std::map<int, Client>::iterator it);
		void handleClientWrite(int cfd, std::map<int, Client>::iterator it);
		void closeIfComplete(int cfd, std::map<int, Client>::iterator it);

	public:
		Server(int port);
		void run();

		std::map<int, Client>& getClients();
		void setListenFd(int newFd);
		int getListenFd() const;
		
};

#endif
