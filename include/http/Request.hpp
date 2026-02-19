/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:17 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/19 12:02:49 by gabrsouz         ###   ########.fr       */
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
		std::string method;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;
		bool isCgi;
	
	public:

		Request();

		// Getters
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHeader(const std::string& name) const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
		bool getIsCgi() const;
		
		// Setters
		void setIsCgi(bool is_Cgi);

		// Main parsing function - full HTTP request
		void parseRequest(const std::string& rawRequest);

	private:
		void setParseVariables(const std::vector<std::string>& parse);
};

#endif
