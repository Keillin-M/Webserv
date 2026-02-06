/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 17:25:57 by gabrsouz         ###   ########.fr       */
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


std::string Response::buildHttpResponse(){
	std::string response;
    
    if (status == 200)
        response += "HTTP/1.1 200 OK\r\n";
	else if (status == 201)
        response += "HTTP/1.1 201 Created\r\n";
	else if (status == 400)
        response += "HTTP/1.1 400 Bad Request\r\n";
    else if (status == 404)
        response += "HTTP/1.1 404 Not Found\r\n";
	else if (status == 500)
        response += "HTTP/1.1 500 Internal Server Error\r\n";
    response += "Content-Type: " + headers["Content-Type"] + "\r\n";
    std::ostringstream oss;
    oss << body.size();
    response += "Content-Length: " + oss.str() + "\r\n";
    response += "\r\n";
    response += body;
    
    return response;
}

std::string Response::getContentType(const std::string& path) {
	if (path.find(".html") != std::string::npos)
        return "text/html";
    if (path.find(".htm") != std::string::npos)
        return "text/html";
    if (path.find(".css") != std::string::npos)
        return "text/css";
    if (path.find(".js") != std::string::npos)
        return "application/javascript";
    if (path.find(".json") != std::string::npos)
        return "application/json";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    if (path.find(".png") != std::string::npos)
        return "image/png";
    if (path.find(".gif") != std::string::npos)
        return "image/gif";
    if (path.find(".txt") != std::string::npos)
        return "text/plain";
    return "application/octet-stream"; //default
}

std::string Response::readFile(const std::string& filepath) {
	std::ifstream file(filepath.c_str());
	if (!file.is_open())
		return "";
	
	std::string content;
	std::string line;
	while (std::getline(file, line)) {
		content += line + "\n";
	}
	file.close();
	return content;
}

bool Response::fileExists(const std::string& filepath) {
	struct stat buffer;
	return (stat(filepath.c_str(), &buffer) == 0);
}

bool Response::isDirectory(const std::string& path) {
	struct stat buffer;
	if (stat(path.c_str(), &buffer) != 0)
		return false;
	return S_ISDIR(buffer.st_mode);
}

std::string Response::handleGet(const std::string& path, const std::string& rootDir) {
	std::string full_path = rootDir;
	if (!full_path.empty() && full_path[full_path.size() - 1] != '/')
		full_path += '/';
	if (!path.empty() && path[0] == '/')
		full_path += path.substr(1);
	else
		full_path += path;
	if (!fileExists(full_path))
        return errorResponse(404, "Not Found");
    if (isDirectory(full_path)) {
        full_path += "/index.html"; //turns the dir in file
        if (!fileExists(full_path))
            return errorResponse(404, "No index file");
    }
	body = readFile(full_path);
    if (body.empty())
        return errorResponse(500, "Failed to read file");
    status = 200;
    headers["Content-Type"] = getContentType(full_path);
    return buildHttpResponse();
}

std::string Response::handlePost(const std::string& requestBody, const std::string& uploadDir) {
	if (requestBody.empty())
        return errorResponse(400, "Bad Request - Empty body");
	std::ostringstream oss;
    oss << uploadDir << "/upload_" << time(NULL) << ".txt";
    std::string filepath = oss.str();
	std::ofstream file(filepath.c_str());
    if (!file.is_open())
        return errorResponse(500, "Failed to create file");
    file << requestBody;
    file.close();
    status = 201;  // Created
    body = "Upload successful";
    headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}

std::string Response::handleDelete(const std::string& path, const std::string& rootDir) {
	std::string full_path = rootDir + path;
    if (!fileExists(full_path))
        return errorResponse(404, "File not found");
    if (isDirectory(full_path))
        return errorResponse(400, "Cannot delete directory");
    if (remove(full_path.c_str()) != 0)
        return errorResponse(500, "Failed to delete");
    status = 200;  // ou 204 No Content
    body = "File deleted successfully";
    headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}

std::string Response::errorResponse(int statusCode, const std::string& message) {
	status = statusCode;
	body = message;
	headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}

