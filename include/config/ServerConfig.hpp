/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:19:06 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 16:52:03 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationConfig.hpp"

class ServerConfig {
	private:
		int port;
		std::string serverName;
		std::string root;
		std::map<int, std::string> errorPages;
		std::vector<LocationConfig> locations;
	
	public:
		ServerConfig();
		
		// Getters
		int getPort() const;
		std::string getServerName() const;
		std::string getRoot() const;
		std::map<int, std::string> getErrorPages() const;
		std::vector<LocationConfig> getLocations() const;

		// Setters
		void setPort(int n);
		void setServerName(const std::string& serverName);
		void setRoot(const std::string& s);
		void addErrorPages(int status, const std::string& path);
		void addLocations(const LocationConfig& location);
};

#endif
