/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/05 16:25:43 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

Response::Response() {}

std::string Response::buildResponse(int statusCode) {
	std::string body;
	std::string statusText;
	(void)status;
	
	if (statusCode == 200) {
		statusText = "OK";
		body = "Hello, world!";
	} else if (statusCode == 404) {
		statusText = "Not Found";
		body = "404 Not found";
	} else {
		statusText = "Internal Server Error";
		body = "500 Internal Server Error";
	}
	
	std::ostringstream oss;
	oss << statusCode;
	std::string statusCodeStr = oss.str();
	
	std::ostringstream oss2;
	oss2 << body.size();
	std::string contentLength = oss2.str();
	
	std::string response;
	response += "HTTP/1.1 " + statusCodeStr + " " + statusText + "\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: " + contentLength + "\r\n";
	response += "\r\n"; //End of header
	response += body;
	
	return response;
}
