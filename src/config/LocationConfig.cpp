/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:18:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/16 15:05:36 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/LocationConfig.hpp"
LocationConfig::LocationConfig() : redirectCode(0) {}

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

std::string LocationConfig::getRedirectUrl() const { return redirectUrl; }

int LocationConfig::getRedirectCode() const { return redirectCode; }

void LocationConfig::setRedirect(int code, const std::string& url) {
	redirectCode = code;
	redirectUrl = url;
}

bool LocationConfig::hasRedirection() const {
	return redirectCode > 0 && !redirectUrl.empty();
}

void LocationConfig::validate() {
	// Fallback to default
	if (allowedMethods.empty())
		allowedMethods.push_back("GET");
	if (indexFile.empty())
		indexFile = "index.html";
	if (path.empty())
		throw std::runtime_error("Location path cannot be empty");
	// Validate redirection codes
	if (redirectCode != 0 && (redirectCode < 300 || redirectCode >= 400))
		throw std::runtime_error("Invalid redirection code. Must be 3xx");
	if (redirectCode > 0 && redirectUrl.empty())
		throw std::runtime_error("Redirection URL cannot be empty");
}
