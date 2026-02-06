/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 14:55:56 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"
#include <sstream>

Request::Request() {}

// Parse initial request line: METHOD SP PATH SP VERSION CRLF
void Request::setParseVariables(const std::string& rawRequest, std::vector<std::string>& parse) {
	method.clear();
	path.clear();
	version.clear();
	headers.clear();
    body.clear();


	// Request Line
	std::istringstream iss(parse[0]);
	iss >> method >> path >> version;

	// Headers
	size_t pos = 0;
	std::string key, value;
	for (size_t i = 1; i < parse.size() - 1; i++) {
		pos = parse[i].find(":");
        	continue;
		key = parse[i].substr(0, pos);
		value = parse[i].substr(pos + 1);
		size_t start = value.find_first_not_of(" \t");
    	if (start != std::string::npos)
    	    value = value.substr(start);
		headers[key] = value;
	}
	
	//Body
	body = parse.back();
}

void Request::parseRequest(const std::string& rawRequest, std::vector<std::string>& parse) {
	parse.clear();
	size_t pos = 0;
	size_t next = rawRequest.find("\r\n");
	size_t body_pos = rawRequest.find("\r\n\r\n");
	std::string line;
	while (pos < body_pos) {
		if (next == std::string::npos || next >= body_pos)
            break;
		line = rawRequest.substr(pos, next - pos);
		parse.push_back(line);
		pos = next + 2;
		next = rawRequest.find("\r\n", pos);
	}
	parse.push_back(rawRequest.substr(body_pos + 4, rawRequest.length() - (body_pos + 4)));
	setParseVariables(rawRequest, parse);
}

std::string Request::getMethod() const { return method; }

std::string Request::getPath() const { return path; }

std::string Request::getVersion() const { return version; }

std::string Request::getHeader(const std::string& name) const {
	
}

std::map<std::string, std::string> Request::getHeaders() const{
	
}

std::string Request::getBody() const {
	
}
