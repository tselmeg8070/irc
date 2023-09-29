#ifndef COMMAND_HPP
# define COMMAND_HPP
# include "ircserv.hpp"

/*
message    =  [ ":" prefix SPACE ] command [ params ] crlf
prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
command    =  1*letter / 3digit
params     =  *14( SPACE middle ) [ SPACE ":" trailing ]
			=/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]

nospcrlfcl =  %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
				; any octet except NUL, CR, LF, " " and ":"
middle     =  nospcrlfcl *( ":" / nospcrlfcl )
trailing   =  *( ":" / " " / nospcrlfcl )

SPACE      =  %x20        ; space character
crlf       =  %x0D %x0A   ; "carriage return" "linefeed"

After extracting the parameter list, all parameters are equal
whether matched by <middle> or <trailing>. <trailing> is just a
syntactic trick to allow SPACE within the parameter.

Commands to parse:
	- NICK <nickname>
*/

/*
Description:
- Gets string and constructs object from it.
Fallback:
- If there is no crlf _command member would be empty string("")
*/
class	Command
{
	private:
		std::string _command;
		std::vector<std::string> _parameters;
	public:
		Command(const std::string &instruction);
		~Command();
		Command(const Command &t);
		Command &operator=(const Command &t);

		std::string			&getCommand();
		std::vector<std::string>	&getParameters();

		static bool hasCrlf(const std::string &instruction);
};

#endif
