/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:17:35 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/10 14:58:21 by kmaeda           ###   ########.fr       */
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
		} else if (std::isspace(c)) {
			handleWhitespace(token, tokens);
		} else if (c == '{' || c == '}' || c == ';') {
			handleSpecialChar(c, token, tokens);
		} else
			token += c;
	}
	if (!token.empty())
		tokens.push_back(token);
	return tokens;
}

ServerConfig ConfigParser::parseServer(const std::vector<std::string>& tokens, size_t& i) {
	ServerConfig config;
	 if (i + 1 >= tokens.size())
        error("Invalid server block: missing opening brace");
    ++i; // skip "server"
    if (tokens[i] != "{")
        error("Expected '{' after 'server'");
    ++i; // skip "{"
	
	while (i < tokens.size() && tokens[i] != "}") {
		if (tokens[i] == "listen") {
			if (i + 1 >= tokens.size())
				error("Missing port value after 'listen'");
			config.setPort(atoi(tokens[++i].c_str()));
			if (i + 1 >= tokens.size() || tokens[i + 1] != ";")
				error("Missing ';' after port number");
			++i; // skip ";"
		} else if (tokens[i] == "root") {
			if (i + 1 >= tokens.size())
				error("Missing root path after 'root'");
			config.setRoot(tokens[++i]);
			if (i + 1 >= tokens.size() || tokens[i + 1] != ";")
				error("Missing ';' after root path");
			++i; //skip ";"
		} else if (tokens[i] == "error_page") {
			if (i + 2 >= tokens.size()) // Need 2 more tokens (code + path)
				error("Missing error code or path after 'error_page'");
			int errorCode = atoi(tokens[++i].c_str());
			config.addErrorPages(errorCode, tokens[++i]);
			if (i + 1 >= tokens.size() || tokens[i + 1] != ";")
				error("Missing ';' after error_page");
			i++;
		} else if (tokens[i] == "location") {
			if (i + 2 >= tokens.size())
				error("Invalid location block syntax");
			LocationConfig location;
			location.setPath(tokens[++i]);
			if (tokens[i] != "{")
				error("Expected '{' after location path");
			++i; // skip "{"
			while (i < tokens.size() && tokens[i] != "}") {
				if (tokens[i] == "root") {
					if (i + 1 >= tokens.size())
						error("Missing root path after 'root' in location");
					location.setRoot(tokens[++i]);
					if (i + 1 >= tokens.size() || tokens[i + 1] != ";")
						error("Missing ';' after root path in location");
					++i; //skip ";"
				} else if (tokens[i] == "allowed_methods") {
					++i;
					while (i < tokens.size() && tokens[i] != ";") {
						location.addAllowedMethods(tokens[i]);
						++i;
					}
					++i; // skip ";"
				} else if (tokens[i] == "index") {
					if (i + 1 >= tokens.size())
						error("Missing index file after 'index' in location");
					location.setIndexFile(tokens[++i]);
					if (i + 1 >= tokens.size() || tokens[i + 1] != ";")
						error("Missing ';' after index file in location");
					++i; // skip ";"
				} else
					++i; // skip unknown token
			}
			if (i >= tokens.size())
				error("Missing '}' for location block");
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
