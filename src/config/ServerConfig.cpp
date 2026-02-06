/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:18:38 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 16:51:52 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"

ServerConfig::ServerConfig() {}

std::string ServerConfig::getServerName() const {}

std::string ServerConfig::getRoot() const {}

std::map<int, std::string> ServerConfig::getErrorPages() const {}

std::vector<LocationConfig> ServerConfig::getLocations() const {}

void ServerConfig::setPort(int n) {
	port = n;
}

void ServerConfig::setServerName(const std::string& serverName) {}

void ServerConfig::setRoot(const std::string& s) {
	root = s;
}

void ServerConfig::addErrorPages(int status, const std::string& path) {
	errorPages[status] = path;
}

void ServerConfig::addLocations(const LocationConfig& location) {
	locations.push_back(location);
}
