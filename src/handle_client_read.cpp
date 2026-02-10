/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_client_read.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:25:08 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/10 15:26:07 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include "../include/Response.hpp"

// Read data from client socket into buffer, handle disconnection/errors
int Server::readClient(int cfd, std::map<int, Client>::iterator &it) {
	char buf[4096];
	ssize_t bytesRead = recv(cfd, buf, sizeof(buf), 0);

	if (bytesRead > 0) {
		it->second.appendRead(buf, static_cast<size_t>(bytesRead));
	} else if (bytesRead == 0) {
		// Peer closed connection (FIN received)
		it->second.setState(CLOSING);
		close(cfd);
		clients.erase(it);
		return 1;
	} else {
		// bytesRead < 0: error — close client (no errno check per subject)
		it->second.setState(CLOSING);
		close(cfd);
		clients.erase(it);
		return 1;
	}
	return 0;
}

// Generate 405 Method Not Allowed response when HTTP method is disallowed
void Server::handleUnallowedMethod(Response &response, std::map<int, Client>::iterator &it, std::string rootDir) {
	response.setErrorPages(config->getErrorPages(), config->getRoot());
	it->second.appendWrite(response.errorResponse(405, "Method Not Allowed"));
	it->second.clearReadBuffer();
}

// Route request to appropriate handler based on HTTP method (GET/POST/DELETE)
void Server::handleMethod(Request &request, Response &response, const LocationConfig* matchedLocation, std::map<int, Client>::iterator &it) {
	std::string httpResponse;
	std::string rootDir = matchedLocation->getRoot();
	std::string indexFile = matchedLocation->getIndexFile();
	if (indexFile.empty())
			indexFile = "index.html";
	std::string uploadDir = rootDir + "/uploads";
	
	// Set error pages for Response object
	response.setErrorPages(config->getErrorPages(), config->getRoot());
	
	if (request.getMethod() == "GET") {
		httpResponse = response.handleGet(request.getPath(), rootDir, indexFile);
	} else if (request.getMethod() == "POST") {
		httpResponse = response.handlePost(request.getBody(), uploadDir);
	} else if (request.getMethod() == "DELETE") {
		// Check if path starts with /upload to determine directory
		std::string path = request.getPath();
		std::string deleteDir = (path.find("upload") != std::string::npos) ? uploadDir : rootDir;
		httpResponse = response.handleDelete(path, deleteDir);
	} else {
		response.setErrorPages(config->getErrorPages(), config->getRoot());
		httpResponse = response.errorResponse(501, "Not Implemented");
	}
	it->second.appendWrite(httpResponse);
	it->second.clearReadBuffer();
}

// Main entry point: read request, validate, find location, and generate response
void Server::handleClientRead(int cfd, std::map<int, Client>::iterator it) {
	if (readClient(cfd, it))
		return;
	if (it->second.requestCompleteCheck()) {
		it->second.setState(WRITING);
		Request request;
		Response response;
		request.parseRequest(it->second.getReadBuffer());
		if (request.getVersion() != "HTTP/1.1") {
			response.setErrorPages(config->getErrorPages(), config->getRoot());
			it->second.appendWrite(response.errorResponse(505, "HTTP Version Not Supported"));
			it->second.clearReadBuffer();
			return;
		}
		const LocationConfig* matchedLocation = config->findMatchLocation(request.getPath());
		if (matchedLocation == NULL) {
			response.setErrorPages(config->getErrorPages(), config->getRoot());
			it->second.appendWrite(response.errorResponse(500, "Internal Server Error"));
			it->second.clearReadBuffer();
			return ;
		} std::string rootDir = matchedLocation->getRoot();
		if (rootDir.empty()) 
			rootDir = config->getRoot();
		if (!matchedLocation->isMethodAllowed(request.getMethod())) {
			handleUnallowedMethod(response, it, rootDir);
			return;
		}
		handleMethod(request, response, matchedLocation, it);
	}
}