/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:58 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/12 12:38:52 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/core/Client.hpp"

// Construction
Client::Client(int fd)
	: fd(fd),
	  readBuffer(),
	  writeBuffer(),
	  keepAlive(false),
	  state(ACCEPTED),
	  lastSeen(std::time(NULL)),
	  headerEndPos(0),
	  contentLength(0),
	  headersParsed(false)
{}

int Client::getFd() const { return fd; }
// Read buffer
void Client::appendRead(const char* data, size_t n) {
	readBuffer.append(data, n);
	updateLastSeen();
}

std::string Client::getReadBuffer() const { return readBuffer; }
// Reset completion tracking — important for keep-alive pipelining
void Client::clearReadBuffer() {
	readBuffer.clear();
	headerEndPos = 0;
	contentLength = 0;
	headersParsed = false;
}
// Two-phase request completion detection
// Phase 1: scan for \r\n\r\n (end of headers), cache position + Content-Length
// Phase 2: verify enough body bytes have arrived
// After headers are found, subsequent calls are O(1) integer comparison.
bool Client::requestCompleteCheck() const {
	if (!headersParsed) {
		size_t pos = readBuffer.find("\r\n\r\n");
		if (pos == std::string::npos)
			return false;
		// Cache results (const_cast: C++98 workaround for mutable-like caching)
		Client* self = const_cast<Client*>(this);
		self->headerEndPos = pos + 4;
		self->contentLength = extractContentLength();
		self->headersParsed = true;
	}
	// No Content-Length: request complete at header end (GET, DELETE, HEAD)
	if (contentLength == 0)
		return true;
	// Wait until all body bytes have arrived
	return (readBuffer.size() - headerEndPos) >= contentLength;
}

// Extract Content-Length from raw headers without full HTTP parsing.
// Only searches within the header portion (up to headerEndPos).
// Checks common casings (HTTP/1.1 headers are case-insensitive).
size_t Client::extractContentLength() const {
	std::string headers = readBuffer.substr(0, headerEndPos);

	std::string needle = "Content-Length:";
	size_t pos = headers.find(needle);
	if (pos == std::string::npos) {
		needle = "content-length:";
		pos = headers.find(needle);
	}
	if (pos == std::string::npos)
		return 0;

	size_t valStart = pos + needle.size();
	size_t valEnd = headers.find("\r\n", valStart);
	if (valEnd == std::string::npos)
		valEnd = headers.size();

	std::string val = headers.substr(valStart, valEnd - valStart);

	// Trim leading whitespace
	size_t first = val.find_first_not_of(" \t");
	if (first == std::string::npos)
		return 0;
	val = val.substr(first);

	long cl = std::atol(val.c_str());
	return (cl > 0) ? static_cast<size_t>(cl) : 0;
}

// Check if request body size exceeds the maximum allowed size
bool Client::isBodySizeExceeded(size_t maxBodySize) const {
	size_t contentLength = extractContentLength();
	return contentLength > maxBodySize;
}

// Write buffer (unchanged behaviour)
void Client::appendWrite(const std::string& s) {
	writeBuffer.append(s);
	updateLastSeen();
}

bool Client::hasWrite() const { return !writeBuffer.empty(); }

std::string& Client::getWriteBuffer() { return writeBuffer; }

// Keep-alive (unchanged behaviour)
void Client::setKeepAlive(bool v) { keepAlive = v; }

bool Client::isKeepAlive() const { return keepAlive; }

// State machine
ClientState Client::getState() const { return state; }

void Client::setState(ClientState s) {
	state = s;
	updateLastSeen();
}

// Activity tracking (foundation for Thursday's timeouts)
void Client::updateLastSeen() { lastSeen = std::time(NULL); }

time_t Client::getLastSeen() const { return lastSeen; }