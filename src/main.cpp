/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/19 11:27:22 by gabrsouz         ###   ########.fr       */
/*                                         
#include <cstdlib>
#include <signal.h>                                   */
/* ************************************************************************** */


#include "../include/core/Server.hpp"
#include "../include/core/ServerManager.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/config/LocationConfig.hpp"
#include <vector>
#include <signal.h>
#include <cstdlib>

// Global flag to control server shutdown
volatile sig_atomic_t g_running = 1;

// Signal handler for graceful shutdown
void signalHandler(int sig) {
	if (sig == SIGINT || sig == SIGTERM)
		g_running = 0;
}

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
	// Setup signal handling for graceful shutdown
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	// Initialize ServerManager with config-parsed servers only
	ServerManager manager;
	manager.initialize(configs);
	manager.run(g_running);
	return 0;
}