/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:17 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/04 16:55:10 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>

class Request {
	private:
		std::string method; // 
		std::string path;
		std::string version;
	
	public:
		Request();	
		
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		void parse(const std::string& rawRequest); //Gab parsing
};

#endif
