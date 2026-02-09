/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 16:08:47 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

Response::Response() {}

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
	// Text types
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
    if (path.find(".xml") != std::string::npos)
        return "application/xml";
    if (path.find(".txt") != std::string::npos)
        return "text/plain";
    
    // Images
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    if (path.find(".png") != std::string::npos)
        return "image/png";
    if (path.find(".gif") != std::string::npos)
        return "image/gif";
    if (path.find(".svg") != std::string::npos)
        return "image/svg+xml";
    if (path.find(".ico") != std::string::npos)
        return "image/x-icon";
    if (path.find(".webp") != std::string::npos)
        return "image/webp";
    
    // Fonts
    if (path.find(".woff") != std::string::npos)
        return "font/woff";
    if (path.find(".woff2") != std::string::npos)
        return "font/woff2";
    if (path.find(".ttf") != std::string::npos)
        return "font/ttf";
    if (path.find(".otf") != std::string::npos)
        return "font/otf";
    
    // Documents
    if (path.find(".pdf") != std::string::npos)
        return "application/pdf";
    if (path.find(".zip") != std::string::npos)
        return "application/zip";
    
    return "application/octet-stream"; //default
}

bool Response::readFile(const std::string& filepath, std::string& contentFile) {
	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;
	std::ostringstream ss;
    ss << file.rdbuf(); // read entire file into stringstream
    contentFile = ss.str();
	return true;
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

bool Response::isSafePath(const std::string& path) {
	// Reject paths containing ".."
	if (path.find("..") != std::string::npos)
		return false;
	// Reject absolute paths (must be relative)
	if (!path.empty() && path[0] != '/')
		return false;
	return true;
}

std::string Response::handleGet(const std::string& path, const std::string& rootDir, const std::string& indexFile) {
	// Security: check for directory traversal
	if (!isSafePath(path))
		return errorResponse(403, "Forbidden");
	
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
        full_path += "/" + indexFile;
        if (!fileExists(full_path))
            return errorResponse(404, "No index file");
    }
    if (!readFile(full_path, body))
        return errorResponse(500, "Failed to read file");
    status = 200;
    headers["Content-Type"] = getContentType(full_path);
    return buildHttpResponse();
}

std::string Response::handlePost(const std::string& requestBody, const std::string& uploadDir) {
	if (requestBody.empty())
        return errorResponse(400, "Bad Request - Empty body");
	mkdir(uploadDir.c_str(), 0755); // Ensure upload directory exists
	std::ostringstream oss;
    oss << uploadDir << "/upload_" << time(NULL) << ".txt";
    std::string filepath = oss.str();
	std::ofstream file(filepath.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open())
        return errorResponse(500, "Failed to create file");
    file.write(requestBody.data(), requestBody.size());
    file.close();
    status = 201;  // Created
    body = "Upload successful";
    headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}

std::string Response::handleDelete(const std::string& path, const std::string& rootDir) {
	// Security: check for directory traversal
	if (!isSafePath(path))
		return errorResponse(403, "Forbidden");
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

