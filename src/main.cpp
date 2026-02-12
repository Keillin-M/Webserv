/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 15:36:51 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/core/Server.hpp"
#include "../include/core/ServerManager.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/config/LocationConfig.hpp"
#include <vector>

int main() {
	std::cout << "=== WEBSERV - Multi-Server ===" << std::endl;
	
	std::vector<ServerConfig> configs;

	// Read config file
	try {
		std::ifstream file("config/test.conf");
		if (!file.is_open()) {
			std::cerr << "Error: Cannot open config/test.conf" << std::endl;
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
		
		std::cout << "Loaded " << configs.size() << " server(s) from config/test.conf" << std::endl;
		
	} catch (const std::exception& e) {
		std::cerr << "Config error: " << e.what() << std::endl;
		return 1;
	}
	
	// Default location (required for all servers)
	LocationConfig defaultLocation;
	defaultLocation.setPath("/");
	defaultLocation.setRoot("config/www");
	defaultLocation.setIndexFile("index.html");
	
	// Create 3 servers on sequential ports
	int ports[] = {8080, 8081, 8082};
	for (int i = 0; i < 3; ++i) {
		ServerConfig config;
		config.setPort(ports[i]);
		config.setRoot("config/www");
		config.addLocations(defaultLocation);
		configs.push_back(config);
	}
	
	ServerManager manager;
	manager.initialize(configs);
	manager.run();
	
	return 0;
}