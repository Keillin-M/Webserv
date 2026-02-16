/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_client_read.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:25:08 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/16 14:17:27 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/core/Server.hpp"
#include "../../include/http/Request.hpp"
#include "../../include/http/Response.hpp"
#include "../../include/cgi/CGI.hpp"

// Read data from client socket into buffer, handle disconnection/errors
int Server::readClient(int cfd, std::map<int, Client>::iterator &it) {
	char buf[4096];
	ssize_t bytesRead = recv(cfd, buf, sizeof(buf), 0);

	if (bytesRead > 0) {
		it->second.appendRead(buf, static_cast<size_t>(bytesRead));
	} else if (bytesRead == 0) {
		// Peer closed connection (FIN received)
		closeClient(cfd);
		return 1;
	} else {
		// bytesRead < 0: error — close client (no errno check per subject)
		closeClient(cfd);
		return 1;
	}
	return 0;
}

// Generate 405 Method Not Allowed response when HTTP method is disallowed
void Server::handleUnallowedMethod(Response &response, std::map<int, Client>::iterator &it) {
	response.setErrorPages(config->getErrorPages(), config->getRoot());
	it->second.appendWrite(response.errorResponse(405, "Method Not Allowed"));
	it->second.clearReadBuffer();
}

void checkIfCgi(Request &request, const LocationConfig* matchedLocation) {
	// Reset and detect if this request should be handled by CGI based on extension
	request.setIsCgi(false);
	std::string reqPath = request.getPath();
	
	// Strip query string before detecting extension
	size_t queryPos = reqPath.find('?');
	if (queryPos != std::string::npos)
		reqPath = reqPath.substr(0, queryPos);
	
	size_t dotPos = reqPath.find_last_of('.');
	std::string ext = (dotPos != std::string::npos) ? reqPath.substr(dotPos) : "";
	std::vector<std::string> cgiExts = matchedLocation->getCGIExtensions();
	for (size_t i = 0; i < cgiExts.size(); ++i) { //check if there is cgi extension
		if (cgiExts[i] == ext) {
			request.setIsCgi(true);
			break;
		}
	}
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
	if (request.getMethod() == "GET" || request.getMethod() == "POST") {
		checkIfCgi(request, matchedLocation);
		if (request.getIsCgi()) 
			httpResponse = response.handleCgi(request, *config, rootDir, matchedLocation->getCGIPath());
		else {
			if (request.getMethod() == "GET")
				httpResponse = response.handleGet(request.getPath(), rootDir, indexFile);
			else
				httpResponse = response.handlePost(request.getBody(), uploadDir);
		}
	} else if (request.getMethod() == "DELETE") {
		// Use rootDir for DELETE - path already contains full path from root
		httpResponse = response.handleDelete(request.getPath(), rootDir);
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

		// Check for HTTP redirection first
		if (matchedLocation->hasRedirection()) {
			response.setErrorPages(config->getErrorPages(), config->getRoot());
			it->second.appendWrite(response.redirectResponse(
				matchedLocation->getRedirectCode(),
				matchedLocation->getRedirectUrl()));
			it->second.clearReadBuffer();
			return;
		}
	
		// Check if method is implemented (before checking if allowed)
		std::string method = request.getMethod();
		if (method != "GET" && method != "POST" && method != "DELETE") {
			response.setErrorPages(config->getErrorPages(), config->getRoot());
			it->second.appendWrite(response.errorResponse(501, "Not Implemented"));
			it->second.clearReadBuffer();
			return;
		}

		if (!matchedLocation->isMethodAllowed(request.getMethod())) {
			handleUnallowedMethod(response, it);
			return;
		}
		handleMethod(request, response, matchedLocation, it);
	}
}