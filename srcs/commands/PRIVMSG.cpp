#include "../../incs/Server.hpp"

void	Server::privmsg(Client *client, std::vector<std::string> tokens)
{
	bool	flag = false;

	if (tokens.size() < 3)
	{
		this->sendError(client, ERR_NEEDMOREPARAMS, tokens);
	}
	else if (tokens.size() == 3)
	{
		if (tokens[1][0] == '#')
		{
			Channel *channel = this->channels[tokens[1]];

			if (channel != NULL)
			{
				flag = true;
				if (this->isClientInChannel(client, channel) == true)
				{
					tokens[2] = ":" + client->getNickName() + " PRIVMSG " + tokens[1] + " :"+ tokens[2] + "\r\n";
					channel->channelSender(tokens[2], client);
				}
				else
				{
					this->sendError(client, ERR_NOTONCHANNEL, tokens);
				}
			}
			else
			{
				flag = false;
			}	
		}
		else
		{
			for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
			{
				if (it->second->getNickName() == tokens[1])
				{
					tokens[2] = ":" + client->getNickName() + " PRIVMSG " + tokens[1] +" :" + tokens[2] + "\r\n";
					this->sender(it->second->getFd(), tokens[2]);
					flag = true;
					break;
				}
			}
		}
		if (flag == false)
		{
			this->sendError(client, ERR_NOSUCHNICK, tokens);
		}
	}
	else
	{
		this->sender(client->getFd(), "Server :invalid parameter counts!\r\n");
	}
}