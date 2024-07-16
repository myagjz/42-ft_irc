#include "../incs/Server.hpp"

Server::Server(int port, std::string password)
{
	this->port = port;
	this->password = password;
	this->socketFd = -1;
	this->sockAddr.sin_family = AF_INET;
	this->sockAddr.sin_port = htons(this->port);
	this->sockAddr.sin_addr.s_addr = INADDR_ANY;


	this->socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd == -1)
		throw std::runtime_error("Server socket could not initilized!");
	if (bind(this->socketFd, (struct sockaddr *)&(this->sockAddr), sizeof(this->sockAddr)) == -1)
		throw std::runtime_error("Server socket could not binded!");
	if (listen(this->socketFd, MAX) == -1)
		throw std::runtime_error("Server socket could not be listened!");

	this->fds[0].fd = this->socketFd;
	this->fds[0].events = POLLIN;
	for (int i = 1; i < MAX; i++)
		this->fds[i].fd = -1;

	std::cout << "Server has been initilized!" << std::endl;
	this->run();
}

void	Server::run()
{
	while (true)
	{
		if (poll(this->fds, MAX, -1) == -1)
			perror("Server could not be polled!");
		if (this->fds[0].revents & POLLIN)
			this->newClient();

		for (int i = 1; i < MAX; i++)
		{
			if (this->fds[i].fd != -1)
			{
				if (this->fds[i].revents & POLLIN)
				{
					this->handler(this->clients[this->fds[i].fd]);
				}

				if (this->fds[i].revents & (POLLHUP | POLLERR))
				{
					std::vector<std::string> ftokens;

					ftokens.push_back("QUIT");
					this->quit(this->clients[this->fds[i].fd],ftokens);
				}	
				fds[i].revents = 0;
			}
		}
	}
}

void	Server::newClient()
{
	struct sockaddr_in	newClientAddr;
	int					newClient;

	newClient = accept(this->socketFd, reinterpret_cast<sockaddr*>(&newClientAddr), reinterpret_cast<socklen_t*>(&newClientAddr));
	if (newClient == -1)
		perror("Client could not be accepted!");
	for (int i = 1; i < MAX; i++)
	{
		if (this->fds[i].fd == -1)
		{	
			if (fcntl(newClient, F_SETFL, O_NONBLOCK) == -1)
				perror("Client fcntl is not accessable!");
			this->fds[i].fd = newClient;
			this->fds[i].events = POLLIN;
			this->clients[this->fds[i].fd] = new Client(this->fds[i].fd);
			sender(this->fds[i].fd, "SERVER You must authanticate!\r\n");
			sender(this->fds[i].fd, "SERVER For help use HELP command!\r\n");
			break;
		}
	}
}

void	Server::handler(Client *client)
{
	char						buff[BUFFER_SIZE];
	int							r_bytes;
	std::vector<std::string>	cmds;

	memset(buff, 0, sizeof(buff));
	r_bytes = recv(client->getFd(), buff, sizeof(buff), 0);
	if (r_bytes > 0)
	{
		std::stringstream	ss(buff);
		std::string			cmd;


		while (std::getline(ss, cmd, '\n'))
		{
			if (cmd.size() <= 0)
				continue;
			if (cmd[cmd.size() - 1] == '\r')
				cmd.erase(cmd.size() - 1);

			std::vector<std::string>	tokens;
			this->parser(&tokens, cmd);
			
			std::cout << "Recived <-------- Fd[" << client->getFd() << "]: " << cmd << std::endl; //for Server console to track

			if (tokens[0] == "NICK")
			{
				this->nick(client, tokens);
			}
			else if (tokens[0] == "USER")
			{
				this->user(client, tokens);
			}
			else if (tokens[0] == "PASS")
			{
				this->pass(client, tokens);
			}
			else if (tokens[0] == "HELP")
			{
				this->help(client, tokens);
			}
			else if (client->getIsRegistered() == false)
			{
				this->sender(client->getFd(), ":Server 451 " + client->getNickName() + " :Register first use command HELP to get help\r\n");
			}
			else if (tokens[0] == "JOIN")
			{
				this->join(client, tokens);
			}
			else if (tokens[0] == "PRIVMSG")
			{
				this->privmsg(client, tokens);
			}
			else if (tokens[0] == "PART")
			{
				this->part(client, tokens);
			}
			else if (tokens[0] == "TOPIC")
			{
				this->topic(client, tokens);
			}
			else if (tokens[0] == "KICK")
			{
				this->kick(client, tokens);
			}
			else if (tokens[0] == "OP")
			{
				this->op(client, tokens);
			}
			else if (tokens[0] == "QUIT")
			{
				this->quit(client, tokens);
			}
		}
	}
	else
	{
		perror("Client could is not be recived!");
	}
}

void	Server::sender(int fd, std::string msg)
{
	std::cout << "Send --------> Fd[" << fd << "]: " << msg << std::endl; //for Server console to track
	send(fd, msg.c_str(), msg.size(), 0);
}

bool	Server::isClientExist(std::string nickName)
{
	for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (nickName == it->second->getNickName())
		{
			return (true);
		}
	}
	return (false);
}

void	Server::checkRegister(Client *client)
{
	if (client->getIsRegistered() == false)
	{
		if (client->getIsPassed() != false && client->getNickName() != "" && client->getUserName() != "")
		{
			client->setIsRegistered(true);
			std::string msg = ":Server 001 " + client->getNickName() + " :Welcome to the Internet Relay Network :" + client->getName() + "\r\n";
			this->sender(client->getFd(), msg);
		}
	}
}

void	Server::parser(std::vector<std::string> *tokens, std::string cmd)
{
	size_t		pos = cmd.find(':');
	std::string	r_str = "";
	std::string	l_str = "";

	if (pos != std::string::npos)
	{
		l_str = cmd.substr(0, pos - 1);
		r_str = cmd.substr(pos, cmd.size());

		if (r_str[0] == ':')
			r_str.erase(0, 1);
		while (r_str[0] == ' ')
			r_str.erase(0, 1);
	}
	else
	{
		l_str = cmd;
	}

	std::stringstream			line(l_str);
	std::string					str;

	while (std::getline(line, str, ' '))
	{
		tokens->push_back(str);
	}

	if (r_str != "")
	{
		tokens->push_back(r_str);
	}
}

void	Server::sendError(Client* client, int code, std::vector<std::string> tokens)
{
	if (code == ERR_NEEDMOREPARAMS)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " " + client->getNickName() + " :Not enough parameters\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_ALREADYREGISTERED)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) +  ":You are already registered\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_PASSWDMISMATCH)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :Password incorrect\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_NONICKNAMEGIVEN)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :No nickname given\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_NICKNAMEINUSE)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :" + tokens[1] + " :Nickname is already in use\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_NOSUCHCHANNEL)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :" + tokens[1] + " :No such a channel\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_NOTONCHANNEL)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :" + tokens[1] + " :Not on channel\r\n";
		this->sender(client->getFd(), msg);
	}
	else if (code == ERR_NOSUCHNICK)
	{
		std::string msg = ":" + tokens[0] + " " + std::to_string(code) + " :" + tokens[1] + " :No such a nick\r\n";
		this->sender(client->getFd(), msg);
	}
}

bool	Server::isClientInChannel(Client *client, Channel *channel)
{
	for (std::vector<Client *>::iterator it = channel->clients.begin(); it != channel->clients.end(); it++)
	{
		if (*it == client)
		{
			return (true);
		}
	}
	return (false);
}

Server::~Server()
{
	std::vector<std::string> tokens;

	tokens.push_back("QUIT");
	for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
	{
		this->quit(it->second, tokens);
		delete (it->second);
	}

	for (std::map<std::string, Channel*>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
	{
		delete (it->second);
	}
	
	close (this->socketFd);
	exit(1);
}
