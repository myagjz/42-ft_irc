#include "../incs/Executor.hpp"
#include "../incs/Channel.hpp"
#include <limits.h>
#include <unistd.h>

Executor::Executor(Server *ptr) 
{
	this->_server = ptr;
	this->_userPtr = NULL;
	this->_mapping["CAP"] = &Executor::_cap;
	this->_mapping["INVITE"] = &Executor::_invite;
	this->_mapping["JOIN"] = &Executor::_join;
	this->_mapping["KICK"] = &Executor::_kick;
	this->_mapping["MODE"] = &Executor::_mode;
	this->_mapping["NICK"] = &Executor::_nick;
	this->_mapping["PASS"] = &Executor::_pass;
	this->_mapping["PRIVMSG"] = &Executor::_privmsg;
	this->_mapping["QUIT"] = &Executor::_quit;
	this->_mapping["TOPIC"] = &Executor::_topic;
	this->_mapping["USER"] = &Executor::_user;
	this->_mapping["BOT"] = &Executor::_privmsg;
	this->_mapping[""] = &Executor::_cap;
	return ;
}

Executor::~Executor(void) 
{
	return ;
}

void Executor::_cap(std::string content)
{
	(void)(content);
	return ;
}

void Executor::execOPs(void) 
{
	size_t size = this->ops.size();
	std::string msg;
	for (size_t i = 0; i < size; i++) 
	{
		if (_mapping.find(ops[i].type) == _mapping.end()) 
		{
			msg = ERR_UNKNOWNCOMMAND(_userPtr, ops[i].type);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			break;
		}
		func f = _mapping[ops[i].type];
		(this->*f)(ops[i].content);	
		ops[i].type = "";
		ops[i].content = "";
	}
}

void Executor::parseBuffer(std::string content) 
{
	std::istringstream iss(content);
	std::string tmp;
	std::string::size_type pos = 0;

	if (!(this->ops.empty())) 
	{
		this->ops.clear();
	}
	while (std::getline(iss, tmp, '\n')) 
	{
		int splitter = tmp.find(' ');
		pos = tmp.find('\r');
		if (pos != std::string::npos)
			tmp = tmp.substr(0, pos);
		std::string type = tmp.substr(0, splitter);
		OP op = {
			.type = tmp.substr(0, splitter),
			.content = tmp.substr(splitter+1, tmp.size())
		};
		this->ops.push_back(op);
	}
}

void Executor::_createChannel(std::string content) 
{
	Channel newChan(content, _userPtr);
	_channels.push_back(newChan);
	std::string msg = RPL_JOIN(_userPtr->getNickname(), content);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_NAMERPLYCREATECHANNEL(_userPtr, newChan, newChan.getNicknameWithPrefix(_userPtr));
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_ENDOFNAMES(newChan.getName());
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
}

bool Executor::isChannel(std::string channel)
{
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		if (channel == (*it).getName())
			return (true);
	return (false);
}

std::string	Executor::nextWord(std::string content)
{
	std::string firstword;
	std::string secword;
	std::istringstream iss(content);

	iss >> firstword >> firstword >> secword;
	if (firstword == secword)
		return ("");
	return (secword);
}

bool	Executor::isDigit(std::string content)
{
	for (int i = 0; content[i] != '\0'; i++)
		if (!isdigit(content[i]))
			return (false);
	return (true);
}

Channel *Executor::getChannelByName(std::string channelName) 
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) 
	{
		if (it->getName() == channelName)
			return &(*it);
	}
	return NULL;
}

bool Executor::isUser(User *user) 
{
		for(std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
		if (user->getNickname() == (*it).getNickname())
			return (true);
	return (false);
}

bool Executor::isUserByNickname(std::string nick) 
{
		for(std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
			if (nick == (*it).getNickname())
				return (true);
	return (false);
}

bool Executor::isOnlySpace(std::string str)
{
	int i = 0;
	while (str[i]) {
		if (str[i] != ' ')
			return false;
		i++;
	}
	return true;
}

void Executor::setUserPtr(User *ptr) 
{
	this->_userPtr = ptr;
}

User *Executor::getUserPtr() 
{
	return _userPtr;
}

User *Executor::getPrivateUserByNickname(std::string nickName)
{

	for (std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++) 
	{
		if ((*it).getNickname() == nickName)
			return &(*it);
	}
	
	return (NULL);
}