/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_client_read.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:25:08 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/12 15:37:26 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http/Response.hpp"
#include "../../include/cgi/CGI.hpp"
#include "../../include/http/Request.hpp"

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
        return errorResponse(403, "Cannot delete directory");
    if (remove(full_path.c_str()) != 0)
        return errorResponse(500, "Failed to delete");
    status = 200;
    body = "File deleted successfully";
    headers["Content-Type"] = "text/plain";
	return buildHttpResponse();
}