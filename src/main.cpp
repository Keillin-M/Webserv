/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 16:40:28 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/core/Server.hpp"
#include "../include/core/ServerManager.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/config/LocationConfig.hpp"
#include <vector>

int main(int argc, char **argv) {
	std::cout << "=== WEBSERV - Multi-Server ===" << std::endl;
	
	// Check command-line arguments
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}
	
	std::vector<ServerConfig> configs;
	std::string configFile = argv[1];

	// Read config file
	try {
		std::ifstream file(configFile.c_str());
		if (!file.is_open()) {
			std::cerr << "Error: Cannot open " << configFile << std::endl;
			return 1;
		}
		
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		file.close();
		
		// Parse config
		ConfigParser parser;
		std::vector<std::string> tokens = parser.tokenize(content);
		parser.parseConfig(tokens);
		configs = parser.getServers();
		
		if (configs.empty()) {
			std::cerr << "Error: No servers found in config" << std::endl;
			return 1;
		}
		
		std::cout << "Loaded " << configs.size() << " server(s) from " << configFile << std::endl;
		
	} catch (const std::exception& e) {
		std::cerr << "Config error: " << e.what() << std::endl;
		return 1;
	}
	
	// Initialize ServerManager with config-parsed servers only
	ServerManager manager;
	manager.initialize(configs);
	manager.run();
	
	return 0;
}