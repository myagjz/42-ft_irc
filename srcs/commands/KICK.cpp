#include "../../incs/Server.hpp"

void	Server::kick(Client *client, std::vector<std::string> tokens)
{
	if (tokens.size() < 3)
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
			Client *kickClient = NULL;

			for (std::map<int, Client *>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
			{
				if (it->second->getNickName() == tokens[2])
					kickClient = it->second;
			}
			if (kickClient == NULL)
			{
				this->sendError(client, ERR_NOSUCHNICK, tokens);
			}
			else
			{
				if (this->isClientInChannel(kickClient, channel) == false)
				{
					this->sendError(client, ERR_NOTONCHANNEL, tokens);
				}
				else
				{
					if (client != channel->admin)
					{
						std::string msg = ":SERVER KICK :You are not operator\r\n";
						this->sender(client->getFd(), msg);
					}
					else
					{
						std::vector<std::string> ftokens;

						ftokens.push_back("PART");
						ftokens.push_back(channel->getName());
						this->part(kickClient, ftokens);
					}
				}
			}
		}
	}
}