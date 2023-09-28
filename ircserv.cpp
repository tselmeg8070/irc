/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadiyamu <tadiyamu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 13:41:47 by tadiyamu          #+#    #+#             */
/*   Updated: 2023/09/28 14:56:42 by tadiyamu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "Client.hpp"


void sendCapLsResponse(int clientSocket) {
	std::string response = "CAP * LS :302\r\n";
	send(clientSocket, response.c_str(), response.length(), 0);
}

void	sendWelcome(int clientSocket)
{
	std::string response = "Welcome to the Internet Relay Network tadiyamu!tadiyamu@localhost\r\n";
	send(clientSocket, response.c_str(), response.length(), 0);
}

void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

int	createServerSocket(int port, std::string password)
{
	int					sockFD;
	int					opt;
	int					nbFDs;
	struct sockaddr_in	serverAddress;
	struct pollfd		fds[MAX_EVENTS];
	int					pollResult;

	(void) password;
	memset(fds, 0, sizeof(fds));
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
	fds[0].fd = sockFD;
	fds[0].events = POLLIN;
	nbFDs = 1;
	std::cout << "Server is listening on port " << port << std::endl;
	std::vector<Client> clients;
	while (true)
	{
		pollResult = poll(fds, nbFDs, -1);
		if (pollResult == -1)
		{
			std::cerr << "Error in poll: " << std::endl;
			break;
		}
		for (int i = 0; i < nbFDs; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == sockFD)
				{
					//Adding new socket client to server
					struct sockaddr_in clientAddress;
					socklen_t clientAddressLength = sizeof(clientAddress);
					int clientSocket = accept(sockFD, (struct sockaddr*)&clientAddress, &clientAddressLength);
					if (clientSocket == -1) {
						std::cerr << "Error accepting client connection"  << std::endl;
						continue;
					}
					Client newClient(clientSocket, "guest");
					clients.push_back(newClient);
					fds[nbFDs].fd = clientSocket;
					fds[nbFDs].events = POLLIN;
					nbFDs++;
					sendCapLsResponse(clientSocket);
				}
				else
				{
					char buffer[1024];
					memset(buffer, 0, 1024);
					int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (bytesRead <= 0)
					{
						close(fds[i].fd);
						clients.erase(clients.begin() + i);
						fds[i].fd = -1;
					}
					else
					{
						std::vector<std::string> tokens;
						const std::string temp = buffer;
						std::cout << temp << std::endl;
						tokenize(temp, ' ', tokens);
						sendWelcome(clients[i].getSocket());
					}
				}
			}
		}
		//shifting fds
		int j = 0;
		for (int i = 0; i < nbFDs; i++) {
			if (fds[i].fd != -1) {
				fds[j] = fds[i];
				j++;
			}
		}
		nbFDs = j;
	}
	return (0);
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
