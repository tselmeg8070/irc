/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadiyamu <tadiyamu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 13:41:47 by tadiyamu          #+#    #+#             */
/*   Updated: 2023/09/29 18:59:27 by tadiyamu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Command.hpp"
#include "ircserv.hpp"

void	sendCapLsResponse(int clientSocket)
{
	std::string response = "CAP * LS :302\r\n";
	send(clientSocket, response.c_str(), response.length(), 0);
}

void	sendWelcome(int clientSocket)
{
	std::string response = "Welcome to the Internet Relay Network tadiyamu!tadiyamu@localhost\r\n";
	send(clientSocket, response.c_str(), response.length(), 0);
}

std::vector<std::string> customSplit(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;
	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start + delimiter.size());
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	if (s.substr(pos_start).size() > 0)
		res.push_back(s.substr(pos_start));
	return (res);
}

int	createServerSocket(int port, std::string password)
{
	int					sockFD;
	int					opt;
	int					nbFDs;
	struct sockaddr_in	serverAddress;
	struct pollfd		fds[MAX_EVENTS];
	int					pollResult;
					struct sockaddr_in clientAddress;
	socklen_t			clientAddressLength;
	int					clientSocket;
					char buffer[1024];
	int					bytesRead;
	int					j;

	(void)password;
	memset(fds, 0, sizeof(fds));
	opt = 1;
	sockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFD == -1)
	{
		std::cerr << "Socket FD error" << std::endl;
		exit(1);
	}
	//to allow multiple connections
	if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
			sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt err to port" << std::endl;
		close(sockFD);
		return (1);
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	if (bind(sockFD, (struct sockaddr *)&serverAddress,
			sizeof(serverAddress)) == -1)
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
			break ;
		}
		for (int i = 0; i < nbFDs; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == sockFD)
				{
					//Adding new socket client to server
					clientAddressLength = sizeof(clientAddress);
					clientSocket = accept(sockFD,
							(struct sockaddr *)&clientAddress,
							&clientAddressLength);
					if (clientSocket == -1)
					{
						std::cerr << "Error accepting client connection" << std::endl;
						continue ;
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
					memset(buffer, 0, 1024);
					bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (bytesRead <= 0)
					{
						close(fds[i].fd);
						clients.erase(clients.begin() + i);
						fds[i].fd = -1;
					}
					else
					{
						std::string temp = buffer;
						std::vector<std::string> instructions = split(temp, "\r\n");
						for (size_t j = 0; j < instructions.size(); j++)
						{
							Command command(instructions[j]);
							for (size_t i = 0; i < instructions[j].size(); i++)
							{
								if (instructions[j][i] == '\r')
									std::cout << "\\r";
								else if (instructions[j][i] == '\n')
									std::cout << "\\n";
								else
									std::cout << instructions[j][i];
							}
							std::cout << std::endl;
							if (command.getCommand().size() != 0)
							{
								std::cout << "Command: " << command.getCommand() << std::endl;
								std::cout << "Parameters: " << std::endl;
								std::vector<std::string> &parameters = command.getParameters();
								for (size_t i = 0; i < parameters.size(); i++)
									std::cout << "- " << parameters[i] << std::endl;
							}
						}
					}
				}
			}
		}
		//shifting fds
		j = 0;
		for (int i = 0; i < nbFDs; i++)
		{
			if (fds[i].fd != -1)
			{
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
	int	port;

	std::string password;
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
