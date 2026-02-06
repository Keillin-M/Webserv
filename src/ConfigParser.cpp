/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:17:35 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/06 12:57:06 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

// Comment: skip
void ConfigParser::skipComment(const std::string& content, size_t& i) {
	while (i < content.size() && content[i] != '\n')
		++i;
}

// Quote
void ConfigParser::handleQuote(char c, bool& quote, std::string& token, std::vector<std::string>& tokens) {
	if (quote) { // Check if closing quote
		token += c;
		tokens.push_back(token);
		token.clear();
		quote = false;
	} else { // Check opening quote 
		if (!token.empty()) {
			tokens.push_back(token);
			token.clear();
		}
		token += c;
		quote = true;
	}
}

// Whitespace ends a token
void ConfigParser::handleWhitespace(std::string& token, std::vector<std::string>& tokens) {
	if (!token.empty()) {
		tokens.push_back(token);
		token.clear();
	}
}

// Special chars as separate token
void ConfigParser::handleSpecialChar(char c, std::string& token, std::vector<std::string>& tokens) {
	if (!token.empty()) {
		tokens.push_back(token);
		token.clear();
	}
	tokens.push_back(std::string(1, c));
}

std::vector<std::string> ConfigParser::tokenize(const std::string& content) {
	std::vector<std::string> tokens;
	std::string token;
	bool quote = false;
	
	for (size_t i = 0; i < content.size(); ++i) {
		char c = content[i];
		
		if (!quote && c == '#') {
			skipComment(content, i);
			continue;
		}
		
		if (c == '"') {
			handleQuote(c, quote, token, tokens);
			continue;
		}
		
		if (quote) { // Everything inside quote is added as 1 token
			token += c;
			continue;
		}

		if (std::isspace(c)) {
			handleWhitespace(token, tokens);
		}
		
		if (c == '{' || c == '}' || c == ';') {
			handleSpecialChar(c, token, tokens);
		}
		else
			token += c;
	}
	if (!token.empty())
		tokens.push_back(token);
	return tokens;
}

ServerConfig::ServerConfig() {}

std::string ServerConfig::getServerName() const {}

std::string ServerConfig::getRoot() const {}

std::map<int, std::string> ServerConfig::getErrorPages() const {}

std::vector<LocationConfig> ServerConfig::getLocations() const {}
	
LocationConfig::LocationConfig() {}
