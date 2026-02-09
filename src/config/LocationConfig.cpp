/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:18:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 15:05:01 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/LocationConfig.hpp"

LocationConfig::LocationConfig() {}

std::string LocationConfig::getPath() const { return path; }

std::vector<std::string> LocationConfig::getAllowedMethods() const { return allowedMethods; }

std::string LocationConfig::getRoot() const { return root; }

std::string LocationConfig::getIndexFile() const { return indexFile; }
		
void LocationConfig::setPath(const std::string& s) { path = s; }

void LocationConfig::addAllowedMethods(const std::string& method) {
	allowedMethods.push_back(method);
}

void LocationConfig::setRoot(const std::string& s) { root = s; }

void LocationConfig::setIndexFile(const std::string& index) { indexFile = index; }

bool LocationConfig::isMethodAllowed(const std::string& method) const {
	
}
