#include "../incs/Executor.hpp"

void Executor::_nick(std::string content)
{
	std::string msg;

	if (_userPtr->checkPassword == false)
	{
		_pass("");
		return;
	}
	if (content.find('#') != std::string::npos || content.find(':') != std::string::npos)
	{
		msg =  ERR_ERRONEUSNICKNAME(content);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	for (std::list<User>::iterator ite = _server->users.begin(); ite != _server->users.end(); ite++)
	{
		if((*ite).getNickname() == content)
		{
			msg = ERR_NICKNAMEINUSE((*ite).getNickname());
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
	}
	if (content == "NICK" || isOnlySpace(content) == true)
	{
		msg = ERR_NONICKNAMEGIVEN();
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (!(_userPtr->getNickname().empty()))
	{
		msg = NICK(_userPtr->getNickname(), content);
		for (std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
			send((*it).getSocket() , msg.c_str(), msg.size(), 0);
	}
	_userPtr->setNickname(content);
}