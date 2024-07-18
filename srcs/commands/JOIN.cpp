#include "../../incs/Server.hpp"

void	Server::join(Client *client, std::vector<std::string> tokens)
{
	if (tokens.size() == 1)
	{
		this->sendError(client, ERR_NEEDMOREPARAMS, tokens);
	}
	else if (tokens.size() == 2)
	{
		Channel	*channel = this->channels[tokens[1]];

		if (tokens[1] == "0")
		{
			for (std::map<std::string, Channel*>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
			{
				if ((it->second) == NULL)
				{
					break;
				}
				if (this->isClientInChannel(client, it->second))
				{
					Channel *channel = it->second;

					channel->removeClient(client);
					this->sender(client->getFd(), ":" + client->getName() + " PART " + channel->getName() + " .\r\n");
					channel->channelSender(":" + client->getName() + " PART " + channel->getName() + " .\r\n", client);
				}
			}
		}
		else if (tokens[1][0] != '#')
		{
			this->sender(client->getFd(), ":SERVER :Channel names should start with #\r\n");
		}
		else if (channel == NULL)
		{
			this->channels[tokens[1]] = new Channel(tokens[1], client);
			channel = this->channels[tokens[1]];
			std::string msg = ":" + client->getName() + " JOIN" + " :" + tokens[1] + "\r\n";
			this->sender(client->getFd(), msg);
			channel->channelSender(msg, client);
		}
		else if (this->isClientInChannel(client, channel) == false)
		{
			std::string msg = ":" + client->getName() + " JOIN" + " :" + tokens[1] + "\r\n";
			channel->channelSender(msg, client);
			this->sender(client->getFd(), msg);
			msg = ":" + client->getUnused() + " 353 " + channel->getName() + " = " + channel->getName() + " :" + channel->getClientNames() + "\r\n";
			this->sender(client->getFd(), msg);
			msg = ":" + client->getUnused() + " 366 " + channel->getName() + " " + channel->getName() + " :end of /NAMES list\r\n";
			this->sender(client->getFd(), msg);
			msg = ":SERVER 332 " + client->getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n";
			this->sender(client->getFd(), msg);
			channel->addClient(client);
		}
	}
	else
	{
		this->sender(client->getFd(), "PASS :invalid parameter counts!\r\n");
	}
}