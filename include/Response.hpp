/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:26 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 17:25:36 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <sys/stat.h>
#include <ctime>
#include <cstdio>
#include <fstream>

class Response {
	private:
		int status;
		std::string body;
		std::map<std::string, std::string> headers;
		
		std::string buildHttpResponse();
		std::string getContentType(const std::string& path);
		std::string readFile(const std::string& filepath);
		bool fileExists(const std::string& filepath);
		bool isDirectory(const std::string& path);
	
	public:
		Response();
		std::string handleGet(const std::string& path, const std::string& rootDir);
		std::string handlePost(const std::string& requestBody, const std::string& uploadDir);
		std::string handleDelete(const std::string& path, const std::string& rootDir);
		std::string errorResponse(int statusCode, const std::string& message);
		std::string buildResponse(int statusCode);
};

#endif
