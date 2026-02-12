/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 13:00:35 by gabrsouz         ###   ########.fr       */
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
	
	// Default location (required for all servers)
	LocationConfig defaultLocation;
	defaultLocation.setPath("/");
	defaultLocation.setRoot("config/www");
	defaultLocation.setIndexFile("index.html");
	defaultLocation.addAllowedMethods("GET");
	defaultLocation.addAllowedMethods("POST");
	defaultLocation.addAllowedMethods("DELETE");
	
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