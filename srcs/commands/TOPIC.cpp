#include "../../incs/Server.hpp"

void	Server::topic(Client *client, std::vector<std::string> tokens)
{
	if (tokens.size() < 2)
	{
		this->sendError(client, ERR_NEEDMOREPARAMS, tokens);
	}
	else
	{
		Channel *channel = this->channels[tokens[1]];

		if (channel == NULL)
		{
			this->sendError(client, ERR_NOSUCHCHANNEL, tokens);
		}
		else
		{
			if (this->isClientInChannel(client, channel) == false)
			{
				this->sendError(client, ERR_NOTONCHANNEL, tokens);
			}
			else
			{
				if (tokens.size() == 2)
				{
					this->sender(client->getFd(), ":Server " + channel->getName() + " :Topic is " + channel->getTopic() + "\r\n");
				}
				else
				{
					if (client == channel->admin)
					{
						std::string msg = ":SERVER 332 " + client->getNickName() + " " + channel->getName() + " :" + tokens[2] + "\r\n";
						this->sender(client->getFd(), msg);
						channel->channelSender(msg, client);
						channel->setTopic(tokens[2]);
					}
					else
					{
						std::string msg = ":SERVER TOPIC :You are not operator\r\n";
						this->sender(client->getFd(), msg);
					}
				}
			}
		}
	}
}