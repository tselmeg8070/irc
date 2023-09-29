#include "Command.hpp"

Command::Command(const std::string &instruction)
{
	std::string	value;
	std::string	trailing;
	size_t		trailingPos;
	std::string 		s;
	bool				flag;

	if (Command::hasCrlf(instruction))
	{
		trailingPos = instruction.find(":");
		if (trailingPos != std::string::npos)
		{
			value = instruction.substr(0, trailingPos - 1);
			trailing = instruction.substr(trailingPos + 1, instruction.size() - trailingPos - 2);
		}
		else
			value = instruction.substr(0, instruction.size() - 2);
		std::stringstream	ss(value);
		flag = false;
		while (std::getline(ss, s, ' '))
		{
			if (!flag)
			{
				_command = s;
				flag = true;
			}
			else
				_parameters.push_back(s);
		}
		if (trailing.size() > 0)
			_parameters.push_back(trailing);
	}
	else
		_command = "";
}

Command::~Command()
{}

Command::Command(const Command &t)
{
	if (this != &t)
	{
		_command = t._command;
		_parameters = t._parameters;
	}
}

Command	&Command::operator=(const Command &t)
{
	if (this != &t)
	{
		_command = t._command;
		_parameters = t._parameters;
	}
	return (*this);
}

std::string			&Command::getCommand()
{
	return (_command);
}

std::vector<std::string>	&Command::getParameters()
{
	return (_parameters);
}

bool	Command::hasCrlf(const std::string &instruction)
{
	if (instruction.size() >= 2)
	{
		if (instruction[instruction.size() - 1] != '\n')
			return (false);
		if (instruction[instruction.size() - 2] != '\r')
			return (false);
		return (true);
	}
	return (false);
}


