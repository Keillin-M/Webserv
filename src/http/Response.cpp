/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 12:37:53 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http/Response.hpp"

Response::Response() {}

// Store error page configuration from server config
void Response::setErrorPages(const std::map<int, std::string>& pages, const std::string& root) {
	errorPages = pages;
	errorRoot = root;
}

// Get custom error page path for a status code, or empty string if not configured
std::string Response::getErrorPagePath(int statusCode) {
	if (errorPages.find(statusCode) != errorPages.end())
		return errorRoot + "/" + errorPages[statusCode];
	return "";
}

// Build complete HTTP response with status line, headers, and body
std::string Response::buildHttpResponse(){
	std::string response;
    
    switch (status) {
        case 200:
            response += "HTTP/1.1 200 OK\r\n";
            break;
        case 201:
            response += "HTTP/1.1 201 Created\r\n";
            break;
        case 400:
            response += "HTTP/1.1 400 Bad Request\r\n";
            break;
        case 403:
            response += "HTTP/1.1 403 Forbidden\r\n";
            break;
        case 404:
            response += "HTTP/1.1 404 Not Found\r\n";
            break;
        case 405:
            response += "HTTP/1.1 405 Method Not Allowed\r\n";
            break;
        case 500:
            response += "HTTP/1.1 500 Internal Server Error\r\n";
            break;
        case 501:
            response += "HTTP/1.1 501 Not Implemented\r\n";
            break;
        case 505:
            response += "HTTP/1.1 505 HTTP Version Not Supported\r\n";
            break;
        default:
            response += "HTTP/1.1 500 Internal Server Error\r\n";
            break;
    }
    response += "Content-Type: " + headers["Content-Type"] + "\r\n";
    std::ostringstream oss;
    oss << body.size();
    response += "Content-Length: " + oss.str() + "\r\n";
    response += "\r\n";
    response += body;
    
    return response;
}

// Determine type based on file extension
std::string Response::getContentType(const std::string& path) {
	// Text types
	if (path.find(".html") != std::string::npos)
        return "text/html";
    if (path.find(".htm") != std::string::npos)
        return "text/html";
    if (path.find(".css") != std::string::npos)
        return "text/css";
    if (path.find(".json") != std::string::npos)
        return "application/json";
    if (path.find(".js") != std::string::npos)
        return "application/javascript";
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

// Read entire file contents into string
bool Response::readFile(const std::string& filepath, std::string& contentFile) {
	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;
	std::ostringstream ss;
    ss << file.rdbuf(); // read entire file into stringstream
    contentFile = ss.str();
	return true;
}

// Check if file or directory exists
bool Response::fileExists(const std::string& filepath) {
	struct stat buffer;
	return (stat(filepath.c_str(), &buffer) == 0);
}

// Check if path is a directory
bool Response::isDirectory(const std::string& path) {
	struct stat buffer;
	if (stat(path.c_str(), &buffer) != 0)
		return false;
	return S_ISDIR(buffer.st_mode);
}

// Validate path to prevent directory traversal attacks
bool Response::isSafePath(const std::string& path) {
	// Reject paths containing ".."
	if (path.find("..") != std::string::npos)
		return false;
	// Reject absolute paths (must be relative)
	if (!path.empty() && path[0] != '/')
		return false;
	return true;
}

// Handle GET request: serve file or index file for directories
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

// Handle CGI requests (stub implementation)
std::string Response::handleCgi(const std::string& path, const std::string& rootDir, const std::string& cgiPath) {
    // Basic validation: CGI interpreter must be configured
    if (cgiPath.empty())
        return errorResponse(500, "CGI interpreter not configured");
    // CGI execution not implemented yet — return 501 for now
    return errorResponse(501, "CGI Not Implemented");
}

// Handle POST request: save uploaded data to file
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

// Handle DELETE request: remove specified file
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
    status = 200;
    body = "File deleted successfully";
    headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}

// Generate error response with custom page if available, plain text otherwise
std::string Response::errorResponse(int statusCode, const std::string& message, const std::string& customErrorPage) {
	status = statusCode;
    body = message;
    
    // Use provided path OR auto-lookup from stored errorPages
    std::string errorPage = customErrorPage.empty() ? getErrorPagePath(statusCode) : customErrorPage;
    
    if (!errorPage.empty() && fileExists(errorPage)) {
        if (readFile(errorPage, body)) {
            headers["Content-Type"] = getContentType(errorPage);
            return buildHttpResponse();
        }
    }
    // Fallback to generic error page
	headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}
