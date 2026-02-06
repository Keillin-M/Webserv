/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:17 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 14:38:35 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Request {
	private:
		std::vector<std::string> parse;
		std::string method;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;
	
	public:
		Request();	
		
		// Getters
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHeader(const std::string& name) const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
		
		// Main parsing function - full HTTP request
		void parseRequest(const std::string& rawRequest, std::vector<std::string>& parse);

	private:
		void setParseVariables(const std::string& rawRequest, std::vector<std::string>& parse);
		
		// TODO: Implement chunked transfer encoding support later
};

#endif
