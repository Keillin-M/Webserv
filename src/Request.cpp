/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:41 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/04 18:27:03 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request() {}

// TODO: Friend implements this (Task 1 - parse first line)
void Request::parse(const std::string& rawRequest) {
	// Stub implementation - friend will replace this
	(void)rawRequest;
	method = "GET";  // Default for now
	path = "/";      // Default for now
	version = "HTTP/1.1";
}

std::string Request::getMethod() const { return method; }

std::string Request::getPath() const { return path; }

std::string Request::getVersion() const { return version; }