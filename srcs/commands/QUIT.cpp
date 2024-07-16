#include "../../incs/Server.hpp"

void	Server::quit(Client *client, std::vector<std::string> tokens)
{
	(void) tokens;

	for (int i = 1; i < MAX; i++)
	{
		if (this->fds[i].fd == client->getFd())
		{
			std::cout << "by" << std::endl;
			std::vector<std::string> ftokens;
			std::string msg = ":" + client->getName() + " QUIT :QUIT\r\n";

			ftokens.push_back("JOIN");
			ftokens.push_back("0");
			this->join(client, ftokens);
			this->sender(client->getFd(), msg);
			this->fds[i].fd = -1;
			this->clients.erase(client->getFd());
			close(client->getFd());
			delete (client);
			break;
		}
	}
}