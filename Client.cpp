#include "Client.hpp"

Client::Client(int socket, const std::string &nickname) : _socket(socket), _nickname(nickname)
{}

Client::~Client()
{}

Client::Client(const Client &t)
{
	if (this != &t)
	{
		_socket = t._socket;
		_nickname = t._nickname;
	}
}

Client	&Client::operator=(const Client &t)
{
	if (this != &t)
	{
		delete this;
		return (*(new Client(t)));
	}
	return (*this);
}

int			Client::getSocket()
{
	return (_socket);
}

std::string	Client::getNickname()
{
	return (_nickname);
}

void		Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}
