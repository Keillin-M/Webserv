/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:17:35 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 17:19:49 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ServerConfig.hpp"
#include "../../include/config/LocationConfig.hpp"
#include <cstdlib>
#include <stdexcept>

ConfigParser::ConfigParser() {}

void ConfigParser::error(const std::string& msg) {
	throw std::runtime_error(msg);
}

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
		} if (c == '"') {
			handleQuote(c, quote, token, tokens);
			continue;
		} if (quote) { // Everything inside quote is added as 1 token
			token += c;
			continue;
		} if (std::isspace(c)) {
			handleWhitespace(token, tokens);
		} if (c == '{' || c == '}' || c == ';') {
			handleSpecialChar(c, token, tokens);
		} else
			token += c;
	}
	if (!token.empty())
		tokens.push_back(token);
	return tokens;
}

void error(const std::string& msg) {
	std::cerr << "ConfigParser error: " << msg << std::endl;
	exit(EXIT_FAILURE);
}


ServerConfig ConfigParser::parseServer(const std::vector<std::string>& tokens, size_t& i) {
	ServerConfig config;
	i += 2;  // skip "server" and "{"
	
	while (i < tokens.size() && tokens[i] != "}") {
		if (tokens[i] == "listen") {
			config.setPort(atoi(tokens[++i].c_str()));
			++i; // skip ";"
		} else if (tokens[i] == "root") { 
			config.setRoot(tokens[++i]);
			++i; //skip ";"
		} else if (tokens[i] == "error_page") {
		int errorCode = atoi(tokens[++i].c_str());
		config.addErrorPages(errorCode, tokens[++i]);
			i++;
		} else if (tokens[i] == "location") {
			LocationConfig location;
			location.setPath(tokens[++i]);
			++i; // skip "{"
			while (tokens[i] != "}") {
				if (tokens[i] == "root") {
					location.setRoot(tokens[++i]);
					++i; //skip ";"
				} else if (tokens[i] == "allowed_methods") {
					++i;
					while (i < tokens.size() && tokens[i] != ";") {
						location.addAllowedMethods(tokens[i]);
						++i;
					}
					++i; // skip ";"
				} else if (tokens[i] == "index") {
					location.setIndexFile(tokens[++i]);
					++i; // skip ";"
				} else
					++i; // skip unknown token
			}
			++i; //skip "}"
			config.addLocations(location);
		} else
			++i;
	}
	++i;
	config.validate();
	return config;
}

void ConfigParser::parseConfig(const std::vector<std::string>& tokens) {
	size_t i = 0;

	while (i < tokens.size()) {
		if (tokens[i] == "server") {
			servers.push_back(parseServer(tokens, i));
		} else
			error("Expected server block");
	}
}
