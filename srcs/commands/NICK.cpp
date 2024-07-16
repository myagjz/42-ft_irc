#include "../../incs/Server.hpp"

void	Server::nick(Client *client, std::vector<std::string> tokens)
{
	if (tokens.size() == 1)
	{
		this->sendError(client, ERR_NONICKNAMEGIVEN, tokens);
	}
	else if (tokens.size() == 2)
	{
		if (this->isClientExist(tokens[1]) == false)
		{
			std::string msg = ":" + client->getName() + " " + tokens[0] + " " + tokens[1] + "\r\n"; 
			client->setNickName(tokens[1]);
			this->checkRegister(client);
			this->sender(client->getFd(), msg);
		}
		else
		{
			this->sendError(client, ERR_NICKNAMEINUSE, tokens);
		}
	}
	else
	{
		this->sender(client->getFd(), "NICK :invalid parameter counts!\r\n");
	}
}
