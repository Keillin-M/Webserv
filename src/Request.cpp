/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/05 16:46:32 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"
#include <sstream>

Request::Request() {}

// Parse initial request line: METHOD SP PATH SP VERSION CRLF
void Request::parse(const std::string& rawRequest) {
	method.clear();
	path.clear();
	version.clear();

	std::string::size_type lineEnd = rawRequest.find("\r\n");
	if (lineEnd == std::string::npos)
		return;

	std::string firstLine = rawRequest.substr(0, lineEnd);
	std::istringstream iss(firstLine);
	iss >> method >> path >> version;
}

std::string Request::getMethod() const { return method; }

std::string Request::getPath() const { return path; }

std::string Request::getVersion() const { return version; }