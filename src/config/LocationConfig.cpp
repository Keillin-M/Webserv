/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:18:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/13 12:18:03 by kmaeda           ###   ########.fr       */
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
	return std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end();
}

std::string LocationConfig::getCGIPath() const { return CGIPath; }

std::vector<std::string> LocationConfig::getCGIExtensions() const {return CGIExtensions; }

void LocationConfig::setCGIExtensions(const std::string& CGIExtension) {
	this->CGIExtensions.push_back(CGIExtension);
}

void LocationConfig::setCGIPath(const std::string& CGIPath) {
	this->CGIPath = CGIPath;
}

void LocationConfig::validate() {
	// Fallback to default
	if (allowedMethods.empty())
		allowedMethods.push_back("GET");
	if (indexFile.empty())
		indexFile = "index.html";
	if (path.empty())
		throw std::runtime_error("Location path cannot be empty");
}
