/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:56:57 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 14:22:36 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>

class Server {
	private:
		int listenFd; //listens the socket fd
		int port;

	public:
		Server(int port);
		void run();
		
};

#endif
