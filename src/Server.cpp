/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmarcos <tmarcos@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 12:57:49 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/02 17:12:51 by tmarcos          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

Server::Server(int port) : listenFd(-1), port(port) {(void)port;}

void Server::run() {
	// Create socket and bind
	listenFd = socket(AF_INET, SOCK_STREAM, 0); //IPv4, TCP, default protocol
	bind(listenFd, (struct sockaddr *)&address, sizeof(address));
	listen(listenFd, SOMAXCONN);
	
	// Fix non-blocking
	int flags = fcntl(listenFd, F_GETFL, 0); // get file status flags
	fcntl(listenFd, F_SETFL, flags | O_NONBLOCK);

	while (true) {
		// ========== STEP 1: Build poll array ==========
		// Em cada iteracao, montamos o array de fds a vigiar
		// Indice 0 = listen fd, indices 1..N = clientes
		std::vector<struct pollfd> pollFds;
		struct pollfd pfd;

		// Adicionar o listen fd (queremos saber quando ha nova conexao)
		pfd.fd = listenFd;
		pfd.events = POLLIN;  // POLLIN = "ha dados para ler" = nova conexao
		pfd.revents = 0;
		pollFds.push_back(pfd);

		// Adicionar cada cliente conectado
		for (size_t i = 0; i < clients.size(); i++) {
			pfd.fd = clients[i];
			pfd.events = POLLIN;  // queremos saber quando o cliente envia dados
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}

		// ========== STEP 2: Wait for activity ==========
		// poll() bloqueia ate algum fd ter actividade (ou timeout)
		// Timeout de 1000ms para nunca ficar pendurado indefinidamente (subject)
		int ready = poll(&pollFds[0], pollFds.size(), 1000);
		if (ready < 0) {
			if (errno == EINTR)  // interrompido por signal, tentar de novo
				continue;
			break;  // erro fatal
		}

		// ========== STEP 3: Handle new connections ==========
		// Se o listen fd tem POLLIN, ha uma conexao na fila -> accept()
		if (pollFds[0].revents & POLLIN) {
			int clientFd = accept(listenFd, NULL, NULL);
			if (clientFd >= 0) {
				// Colocar o novo cliente em non-blocking (subject obriga)
				int clientFlags = fcntl(clientFd, F_GETFL, 0);
				fcntl(clientFd, F_SETFL, clientFlags | O_NONBLOCK);
				// Adicionar ao vector de clientes
				clients.push_back(clientFd);
			}
		}

		// ========== STEP 4: Handle client data ==========
		// Percorrer clientes e verificar quais tem dados para ler
		for (size_t i = 0; i < clients.size(); i++) {
			// pollFds[i+1] porque indice 0 e o listen fd
			if (!(pollFds[i + 1].revents & POLLIN))
				continue;

			// Ler dados do cliente
			char buf[4096]; //tamanho alinhado com a memoria do sistema
			ssize_t bytesRead = recv(clients[i], buf, sizeof(buf), 0);

			if (bytesRead > 0) {				// TODO: guardar no buffer do cliente, fazer parse do request
				(void)buf;
			} else {
				// bytesRead <= 0: cliente desconectou ou erro
				// Fechar fd e remover do vector (swap com ultimo + pop)
				close(clients[i]);
				clients[i] = clients.back();
				clients.pop_back();
				i--;  // para nao saltar o elemento que veio do fim
			}
		}
	}
}
