#include "../../incs/Server.hpp"

void	Server::user(Client *client, std::vector<std::string> tokens)
{
	if (client->getUserName() != "")
	{
		this->sendError(client, ERR_ALREADYREGISTERED, tokens);
	}
	else if (tokens.size() == 5)
	{
		client->setUserName(tokens[1]);
		client->setMode(tokens[2]);
		client->setUnused(tokens[3]);
		client->setRealName(tokens[4]);
		this->checkRegister(client);
	}
	else
	{
		this->sender(client->getFd(), "USER :invalid parameter counts!\r\n");
	}
}