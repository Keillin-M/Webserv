/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmarcos <tmarcos@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:08 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/10 12:26:48 by tmarcos          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>

enum ClientState {
	ACCEPTED,   // just acceptedt
	READING,    // receiving request (headers + body)
	WRITING,    // sending response
	IDLE,       // keep-alive, waiting for next request
	CLOSING     // scheduled for close
};

class Client {
	private:
		int fd; // client socket fd
		std::string readBuffer; // incoming data buffer
		std::string writeBuffer; // outgoing data buffer
		bool keepAlive;
		
		// Lifecycle tracking
		ClientState state;
		time_t lastSeen;

		// Request completion tracking (two-phase detection)
		size_t headerEndPos;    // cached position after \r\n\r\n
		size_t contentLength;   // extracted from Content-Length header
		bool headersParsed;     // avoid re-scanning on every recv

		// Internal: extract Content-Length from raw headers
		size_t extractContentLength() const;

	public:
		Client(int fd);
		int getFd() const;

		// read buffer accessors
		void appendRead(const char* data, size_t n);
		std::string getReadBuffer() const;
		bool requestCompleteCheck() const;
		void clearReadBuffer();
		bool isBodySizeExceeded(size_t maxBodySize) const;

		// write buffer accessors
		void appendWrite(const std::string& s);
		bool hasWrite() const;
		std::string& getWriteBuffer();
		void setKeepAlive(bool v);
		bool isKeepAlive() const;

		// State machine
		ClientState getState() const;
		void setState(ClientState s);

		// Activity tracking
		time_t getLastSeen() const;
		void updateLastSeen();
};

#endif
