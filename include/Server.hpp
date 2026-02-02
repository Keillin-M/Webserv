/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmarcos <tmarcos@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:56:57 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 17:54:15 by tmarcos          ###   ########.fr       */
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

class Client;

class Server {
	private:
		int listenFd; //listens the socket fd
		int port;
		struct sockaddr address; //socket address
		std::map<int, Client> clients;

	public:
		Server(int port);
		void run();
		
};

#endif
