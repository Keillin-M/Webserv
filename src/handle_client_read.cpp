/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_client_read.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:25:08 by gabrsouz          #+#    #+#             */
/*   Updated: 2026/02/10 12:40:12 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include "../include/Response.hpp"


void Server::emptyMatchLocation(Response &response, std::map<int, Client>::iterator &it) {
	std::string errorPagePath;
	std::map<int, std::string> errorPages = config->getErrorPages();
	if (errorPages.find(500) != errorPages.end())
		errorPagePath = config->getRoot() + "/" + errorPages[500];
	it->second.appendWrite(response.errorResponse(500, "Internal Server Error", errorPagePath));
	it->second.clearReadBuffer();
}

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

void Server::handleUnallowedMethod(Response &response, std::map<int, Client>::iterator &it, std::string rootDir) {
	std::string errorPagePath;
	std::map<int, std::string> errorPages = config->getErrorPages();
	if (errorPages.find(405) != errorPages.end()) {
		errorPagePath = rootDir + "/" + errorPages[405];
	}
	it->second.appendWrite(response.errorResponse(405,"Method Not Allowed", errorPagePath));
	it->second.clearReadBuffer();
}

void Server::handleMethod(Request &request, Response &response, const LocationConfig* matchedLocation, std::map<int, Client>::iterator &it) {
	std::string httpResponse;
	std::string rootDir = matchedLocation->getRoot();
	std::string indexFile = matchedLocation->getIndexFile();
	if (indexFile.empty())
			indexFile = "index.html";
	std::string uploadDir = rootDir + "/uploads";
	if (request.getMethod() == "GET") {
		httpResponse = response.handleGet(request.getPath(), rootDir, indexFile);
	} else if (request.getMethod() == "POST") {
		httpResponse = response.handlePost(request.getBody(), uploadDir);
	} else if (request.getMethod() == "DELETE") {
		// Check if path starts with /upload to determine directory
		std::string path = request.getPath();
		std::string deleteDir = (path.find("upload") != std::string::npos) ? uploadDir : rootDir;
		httpResponse = response.handleDelete(path, deleteDir);
	} else
		httpResponse = response.errorResponse(501, "Method not allowed");
	it->second.appendWrite(httpResponse);
	it->second.clearReadBuffer();
}

void Server::handleClientRead(int cfd, std::map<int, Client>::iterator it) {
	if (readClient(cfd, it))
		return;
	if (it->second.requestCompleteCheck()) {
		it->second.setState(WRITING);
		Request request;
		Response response;
		request.parseRequest(it->second.getReadBuffer());
		if (request.getVersion() != "HTTP/1.1") {
			it->second.appendWrite(response.errorResponse(505, "HTTP Version Not Supported"));
			it->second.clearReadBuffer();
			return;
		}
		const LocationConfig* matchedLocation = config->findMatchLocation(request.getPath());
		if (matchedLocation == NULL) {
			emptyMatchLocation(response, it);
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