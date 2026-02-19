/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:19:06 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/19 11:55:25 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include "LocationConfig.hpp"

class ServerConfig {
	private:
		int port;
		std::string serverName;
		std::string root;
		std::map<int, std::string> errorPages;
		std::vector<LocationConfig> locations;
		size_t clientMaxBodySize; // in bytes
	
	public:
		ServerConfig();
		
		// Getters
		int getPort() const;
		std::string getServerName() const;
		std::string getRoot() const;
		std::map<int, std::string> getErrorPages() const;
		std::vector<LocationConfig> getLocations() const;
		size_t getClientMaxBodySize() const;

		// Setters
		void setPort(int n);
		void setServerName(const std::string& serverName);
		void setRoot(const std::string& s);
		void addErrorPages(int status, const std::string& path);
		void addLocations(const LocationConfig& location);
		void setClientMaxBodySize(const std::string& sizeStr);

		const LocationConfig* findMatchLocation(const std::string& requestPath) const;
		void validate();
};

#endif
