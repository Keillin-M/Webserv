/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:26 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/10 15:17:08 by kmaeda           ###   ########.fr       */
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
		std::map<int, std::string> errorPages;
		std::string errorRoot;
		
		std::string buildHttpResponse();
		std::string getContentType(const std::string& path);
		std::string getErrorPagePath(int statusCode);
		bool readFile(const std::string& filepath, std::string& contentFile);
		bool fileExists(const std::string& filepath);
		bool isDirectory(const std::string& path);
		bool isSafePath(const std::string& path);
	
	public:
		Response();
		
		void setErrorPages(const std::map<int, std::string>& pages, const std::string& root);

		std::string handleGet(const std::string& path, const std::string& rootDir, const std::string& indexFile);
		std::string handlePost(const std::string& requestBody, const std::string& uploadDir);
		std::string handleDelete(const std::string& path, const std::string& rootDir);
		std::string errorResponse(int statusCode, const std::string& message, const std::string& customErrorPage = "");
};

#endif
