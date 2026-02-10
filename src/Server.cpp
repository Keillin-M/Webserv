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