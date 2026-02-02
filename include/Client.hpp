/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:08 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 14:15:13 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

class Client {
	private:
		int fd; // client socket fd
		std::string readBuffer; // placeholder for incoming data
		std::string writeBuffer; // placeholder for outcoming data
		
	public:
		Client(int fd);
		int getFd() const; //normal getter
};

#endif
