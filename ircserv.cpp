/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadiyamu <tadiyamu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 13:41:47 by tadiyamu          #+#    #+#             */
/*   Updated: 2023/09/21 14:42:19 by tadiyamu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	createServerSocket(int port, std::string password)
{
	int					sockFD;
	int					opt;
	struct sockaddr_in	serverAddress;

	(void) password;
	opt = 1;
	sockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFD == -1)
	{
		std::cerr << "Socket FD error" << std::endl;
		exit(1);
	}
	//to allow multiple connections
	if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt err to port" << std::endl;
		close(sockFD);
		return (1);
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	if (bind(sockFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1)
	{
		std::cerr << "Socket binding err to port" << std::endl;
		close(sockFD);
		return (1);
	}
	if (listen(sockFD, 10) == -1)
	{
		std::cerr << "Error listening for connections" << std::endl;
		close(sockFD);
		return (1);
	}
	std::cout << "Server is listening on port " << port << std::endl;
	while (true)
	{
		struct sockaddr_in clientAddress;
		socklen_t clientAddressLength = sizeof(clientAddress);
		int clientSocket = accept(sockFD, (struct sockaddr*)&clientAddress, &clientAddressLength);
		if (clientSocket == -1) {
			std::cerr << "Error accepting client connection"  << std::endl;
			continue;
		}

		// Handle the new client connection here
		// You can spawn a new thread or process to handle each client

		close(clientSocket);
	}
}

int	main(int argc, char **argv)
{
	int			port;
	std::string	password;

	if (argc != 3)
	{
		std::cerr << "Wrong number of args" << std::endl;
		return (1);
	}
	port = std::atoi(argv[1]);
	password = argv[2];
	std::cout << "Port: " << port << std::endl;
	std::cout << "Pass: " << password << std::endl;
	createServerSocket(port, password);
	return (0);
}
