#include "../../incs/Server.hpp"

void	Server::help(Client *client, std::vector<std::string> tokens)
{
	(void) tokens;

	if (client->getIsRegistered() == false)
	{
		this->sender(client->getFd(), "Server :At command explanation <values> are values without < and >\r\n");
		this->sender(client->getFd(), "Server :At command explanation [values] are optional\r\n");
		this->sender(client->getFd(), "Server :Syntax : is used for last paramaters not divide words by space and take as a one parameter\r\n");
		this->sender(client->getFd(), "Server :-------------------------------------------------------------:\r\n");
		this->sender(client->getFd(), "Server :To set your nick name use NICK <nickName>\r\n");
		this->sender(client->getFd(), "Server :To set your user name use USER <userName> <mode> <unused> :<realName>\r\n");
		this->sender(client->getFd(), "Server :To enter the password use PASS <password>\r\n");
	}
	else
	{
		this->sender(client->getFd(), "Server :At command explanation <values> are values without < and >\r\n");
		this->sender(client->getFd(), "Server :At command explanation [values] are optional\r\n");
		this->sender(client->getFd(), "Server :Syntax : is used for last paramaters not divide words by space and take as a one parameter\r\n");
		this->sender(client->getFd(), "Server :-------------------------------------------------------------:\r\n");
		this->sender(client->getFd(), "Server :To join a channel use JOIN <nicklName> <msg>\r\n");
		this->sender(client->getFd(), "Server :To leave a channel use PART <channelName>\r\n");
		this->sender(client->getFd(), "Server :To message a channel or a client use PRIVMSG <nickName> :<msg>\r\n");
		this->sender(client->getFd(), "Server :To quit use QUIT\r\n");
	}
}