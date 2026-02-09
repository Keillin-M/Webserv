/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:18:38 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 15:58:22 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"

ServerConfig::ServerConfig() {}

int ServerConfig::getPort() const { return port; }

std::string ServerConfig::getServerName() const { return serverName; }

std::string ServerConfig::getRoot() const { return root; }

std::map<int, std::string> ServerConfig::getErrorPages() const { return errorPages; }

std::vector<LocationConfig> ServerConfig::getLocations() const { return locations; }

void ServerConfig::setPort(int n) { port = n; }

void ServerConfig::setServerName(const std::string& s) { serverName = s; }

void ServerConfig::setRoot(const std::string& s) { root = s; }

void ServerConfig::addErrorPages(int status, const std::string& path) { 
	errorPages[status] = path;
}

void ServerConfig::addLocations(const LocationConfig& location) {
	locations.push_back(location);
}

const LocationConfig* ServerConfig::findMatchLocation(const std::string& requestPath) const {
	const LocationConfig* bestMatch = NULL;
	size_t longestLength = 0;

	for (std::vector<LocationConfig>::const_iterator it = locations.begin(); 
		it != locations.end();
		++it) {
		std::string locationPath = it->getPath();
		
		//Check if locationPath prefix match requestPath
		if (requestPath.find(locationPath) == 0) {
			if (locationPath.length() > longestLength) {
				longestLength = locationPath.length();
				bestMatch = &(*it); // Get pointer to the LocationConfig
			}
		}
	}
	return bestMatch;
}
