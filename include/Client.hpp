/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:08 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/03 11:22:15 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

class Client {
	private:
		int fd; // client socket fd
		std::string readBuffer; // incoming data buffer
		std::string writeBuffer; // outgoing data buffer
		bool keepAlive;

	public:
		Client(int fd);
		int getFd() const;

		// read buffer accessors
		void appendRead(const char* data, size_t n);
		std::string getReadBuffer() const;

		// write buffer accessors
		void appendWrite(const std::string& s);
		bool hasWrite() const;
		std::string& getWriteBuffer();
		void setKeepAlive(bool v);
		bool isKeepAlive() const;

};

#endif
