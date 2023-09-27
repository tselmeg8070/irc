#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ircserv.hpp"

class	Client
{
	private:
		int			_socket;
		std::string _nickname;
	public:
		Client(int socket, const std::string &nickname);
		~Client();
		Client(const Client &t);
		Client &operator=(const Client &t);

		int			getSocket();
		std::string	getNickname();
		void		setNickname(const std::string &nickname);
};

#endif
