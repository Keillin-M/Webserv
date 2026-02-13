/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 14:40:53 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/13 15:03:55 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cgi/CGI.hpp"
#include "../../include/http/Request.hpp"
#include "../../include/config/ServerConfig.hpp" 

CGI::CGI() {}

CGI::~CGI() {}

std::string CGI::getQuery(const std::string& path) {
    size_t pos = path.find('?');
    if (pos == std::string::npos) {
        return "";
    }
    
    std::string query;
    for (size_t i = pos + 1; i < path.length(); ++i) {
        query += path[i];
    }
    return query;
}

std::string CGI::getPathInfo(const std::string& requestPath, const std::string& scriptName) {
	size_t queryPos = requestPath.find('?');
	
	std::string pathOnly;
	    
    size_t endPos = (queryPos != std::string::npos) ? queryPos : requestPath.length();
    for (size_t i = 0; i < endPos; ++i) {
        pathOnly += requestPath[i];
    }
    
    // Check if starts with scriptName
    if (pathOnly.find(scriptName) == 0) {
        std::string pathInfo;
        for (size_t i = scriptName.length(); i < pathOnly.length(); ++i) {
            pathInfo += pathOnly[i];
        }
        return pathInfo;
    }
    
    return "";
}

char** CGI::createEnvArray() {
	char** envp = new char*[env.size() + 1];
	int i = 0;
	
	for (std::map<std::string, std::string>::const_iterator it = env.begin(); 
      it != env.end(); ++it) {
     std::string entry = it->first + "=" + it->second;
     envp[i] = new char[entry.length() + 1];
     std::strcpy(envp[i], entry.c_str());
     i++;
    }
	envp[i] = NULL;
	return envp;
}

void CGI::freeEnvArray(char** envp) {
    for (int i = 0; envp[i] != NULL; ++i) {
        delete[] envp[i];
    }
    delete[] envp;
}

static std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

static std::string toUpperUnderscore(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '-') result += '_';
        else result += std::toupper(c);
    }
    return result;
}

void CGI::buildEnvironment(const Request& req, const ServerConfig& server, 
    const std::string& scriptPath) {
    // Required CGI meta-variables
    env["REQUEST_METHOD"] = req.getMethod();
    env["QUERY_STRING"] = getQuery(req.getPath());
    env["SCRIPT_NAME"] = scriptPath;
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    
    // Server info
    env["SERVER_NAME"] = server.getServerName();
    env["SERVER_PORT"] = intToString(server.getPort());
    
    // Content headers (if present)
    std::string contentType = req.getHeader("Content-Type");
    if (!contentType.empty()) {
        env["CONTENT_TYPE"] = contentType;
    }
    
    if (req.getMethod() == "POST") {
        env["CONTENT_LENGTH"] = intToString(req.getBody().length());
    }
    
    // PATH_INFO
    env["PATH_INFO"] = getPathInfo(req.getPath(), scriptPath);
    
    // Convert all HTTP headers to HTTP_* variables
    std::map<std::string, std::string> headers = req.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it) {
        std::string envName = "HTTP_" + toUpperUnderscore(it->first);
        env[envName] = it->second;
    }
}

std::string CGI::execute(const Request& req, const ServerConfig& server, 
	const std::string& scriptPath, const std::string& interpreterPath) {
	int pipeIn[2], pipeOut[2];
	pipe(pipeIn); // Sending body to script
	pipe(pipeOut); // Reading script output

	buildEnvironment(req, server, scriptPath);
	char** envp = createEnvArray();	
	
	// Create args array for execve
	char* args[3];
	args[0] = const_cast<char*>(interpreterPath.c_str());
	args[1] = const_cast<char*>(scriptPath.c_str());
	args[2] = NULL;
	
	pid_t pid = fork();
	if (pid == 0) {
		// Redirect stdin/stdout
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		
		// Close unused ends
		close(pipeIn[1]);
		close(pipeOut[0]);
		
		// Execute script
		execve(interpreterPath.c_str(), args, envp);
		exit(1);
	}
	
	close(pipeIn[0]);
	close(pipeOut[1]);
	
	write(pipeIn[1], req.getBody().c_str(), req.getBody().length());
	close(pipeIn[1]);

	std::string output;
	char buffer[4096];
	ssize_t bytesRead;
	
	while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytesRead);
	}
	close(pipeOut[0]);

	waitpid(pid, NULL, 0);
	freeEnvArray(envp);
	return output;
}