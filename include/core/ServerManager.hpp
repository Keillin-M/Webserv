/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 12:34:26 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/12 12:45:50 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <map>
#include <set>
#include <signal.h>
#include "Client.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "config/ServerConfig.hpp"
#include "../core/Server.hpp"


class ServerManager {
	private:
		std::vector<Server*> servers;
		std::map<int, Server*> listenFdToServer;
		std::map<int, Server*> clientFdToServer;
		
		void buildPollArray(std::vector<struct pollfd>& fds);
		void handleListenSocket(int fd);
		void handleClientSocket(int fd, short revents);
		void validatePorts(std::vector<ServerConfig>& configs);
		void updateClientMapping(Server* server);
		
	public:
		ServerManager();
		~ServerManager();
		
		void initialize(std::vector<ServerConfig>& configs);
		void run(volatile sig_atomic_t& running);
		void shutdown();
};

#endif
