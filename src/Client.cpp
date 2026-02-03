/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabrsouz <gabrsouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:58 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/03 11:22:19 by gabrsouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(int fd) : fd(fd), readBuffer(), writeBuffer(), keepAlive(false) {}

int Client::getFd() const { return fd;}

void Client::appendRead(const char* data, size_t n) { readBuffer.append(data, data + n); }

std::string Client::getReadBuffer() const { return readBuffer; }

void Client::appendWrite(const std::string& s) { writeBuffer.append(s); }		
bool Client::hasWrite() const { return !writeBuffer.empty(); }
std::string& Client::getWriteBuffer() { return writeBuffer; }
void Client::setKeepAlive(bool v) { keepAlive = v; }
bool Client::isKeepAlive() const { return keepAlive; }