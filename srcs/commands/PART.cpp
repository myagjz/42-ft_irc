#include "../../incs/Server.hpp"

void	Server::part(Client *client, std::vector<std::string> tokens)
{
	if (tokens.size() == 2 || tokens.size() == 3)
	{
		Channel *channel = this->channels[tokens[1]];

		if (channel != NULL)
		{
			if (this->isClientInChannel(client, channel) == true)
			{
				channel->removeClient(client);
				this->sender(client->getFd(), ":" + client->getName() + " PART " + channel->getName() + " .\r\n");
				channel->channelSender(":" + client->getName() + " PART " + channel->getName() + " .\r\n", client);
				if (tokens.size() == 3)
				{
					tokens[2] = client->getNickName() + " :" + tokens[2] + "\r\n";
					channel->channelSender(tokens[2], client);
				}
			}
			else
			{
				this->sendError(client, ERR_NOTONCHANNEL, tokens);
			}
		}
		else
		{
			this->sendError(client, ERR_NOSUCHCHANNEL, tokens);
		}
	}
	else
	{
		this->sender(client->getFd(), "Server :invalid parameter counts!\r\n");
	}
}