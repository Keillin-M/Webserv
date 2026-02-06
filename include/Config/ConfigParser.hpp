/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:07:52 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 13:08:27 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>

class ConfigParser {
	private:
		std::vector<ServerConfig> servers;
		std::string readFile(const char* path);
		void skipComment(const std::string& content, size_t& i);
		void handleQuote(char c, bool& quote, std::string& token, std::vector<std::string>& tokens);
		void handleWhitespace(std::string& token, std::vector<std::string>& tokens);
		void handleSpecialChar(char c, std::string& token, std::vector<std::string>& tokens);
		
	public:
		ConfigParser();
	
		std::vector<std::string> tokenize(const std::string& content);
		void parse(const std::string& content);
		std::vector<ServerConfig> getServers();
};

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
		void setPort(int port);
		void setServerName(const std::string& serverName);
		void setRoot(const std::string& root);
		void addErrorPages(const std::string& errorPages);
		void addLocations(const std::string& locations);
};

class LocationConfig {
	private:
		std::string path;
		std::vector<std::string> allowedMethods;
		std::string root;
		std::string indexFile;
	
	public:
		LocationConfig();

		// Getters
		std::string getPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getRoot() const;
		std::string getIndexFile() const;

		// Setters
		void setPath(const std::string& path);
		void addAllowedMethods(const std::string& method);
		void setRoot(const std::string& root);
		void setIndexFile(const std::string& index);
};

#endif
