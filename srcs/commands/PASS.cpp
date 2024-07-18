#include "../../incs/Server.hpp"

void	Server::pass(Client *client, std::vector<std::string> tokens)
{
	if (client->getIsPassed() == true)
	{
		this->sendError(client, ERR_ALREADYREGISTERED, tokens);
	}
	else
	{
		if (tokens.size() == 2)
		{
			if (tokens[1] == this->password)
			{
				client->setIsPassed(true);
				this->checkRegister(client);
			}
			else
			{
				this->sendError(client, ERR_PASSWDMISMATCH, tokens);
			}
		}
		else
		{
			this->sender(client->getFd(), "PASS :invalid parameter counts!\r\n");
		}
	}
}
