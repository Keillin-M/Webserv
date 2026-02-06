/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:17:35 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/05 17:51:06 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ServerConfig::ServerConfig() {}

std::string ServerConfig::getServerName() const {}

std::string ServerConfig::getRoot() const {}

std::map<int, std::string> ServerConfig::getErrorPages() const {}

std::vector<LocationConfig> ServerConfig::getLocations() const {}
	
LocationConfig::LocationConfig() {}
