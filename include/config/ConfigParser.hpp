/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:07:52 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 15:16:44 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <iostream>

class ServerConfig;

class ConfigParser {
	private:
		std::vector<ServerConfig> servers;
		
		std::string readFile(const char* path);
		void skipComment(const std::string& content, size_t& i);
		void handleQuote(char c, bool& quote, std::string& token, std::vector<std::string>& tokens);
		void handleWhitespace(std::string& token, std::vector<std::string>& tokens);
		void handleSpecialChar(char c, std::string& token, std::vector<std::string>& tokens);
		void error(const std::string& msg);
		ServerConfig parseServer(const std::vector<std::string>& tokens, size_t& i);

	public:
		ConfigParser();
	
		std::vector<std::string> tokenize(const std::string& content);
		void parseConfig(const std::vector<std::string>& tokens);
		std::vector<ServerConfig> getServers() const;
};

#endif
