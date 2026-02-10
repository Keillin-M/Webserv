/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:56:57 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/10 12:39:42 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "config/ServerConfig.hpp"

class Server {
	private:
		int listenFd;
		int port;
		std::map<int, Client> clients;
		ServerConfig* config;

	public:
		Server(int port);
		Server(int port, ServerConfig* cfg);
		
		void run();
		void setupListenSocket();
		void createPollFds(std::vector<struct pollfd>& pollFds, struct pollfd pfd);
		void acceptNewClients();
		void handleClientRead(int cfd, std::map<int, Client>::iterator it);
		int readClient(int cfd, std::map<int, Client>::iterator &it);
		void emptyMatchLocation(Response &response, std::map<int, Client>::iterator &it);
		void handleUnallowedMethod(Response &response, std::map<int, Client>::iterator &it, std::string rootDir);
		void handleMethod(Request &request, Response &response, const LocationConfig* matchedLocation, std::map<int, Client>::iterator &it);
		void handleClientWrite(int cfd, std::map<int, Client>::iterator it);
		void closeIfComplete(int cfd, std::map<int, Client>::iterator it);

		std::map<int, Client>& getClients();
		void setListenFd(int newFd);
		int getListenFd() const;
		int getPort() const;
		ServerConfig* getConfig();
};

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
		void run();
};

#endif
