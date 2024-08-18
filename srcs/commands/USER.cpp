#include "../incs/Executor.hpp"

void Executor::_user(std::string content)
{
	std::istringstream iss(content);
	std::string tmp, msg;
	std::vector<std::string> params;

	if (_userPtr->checkPassword == false)
	{
		_pass("");
		return;
	}
	if (_userPtr->getNickname().empty())
	{
		_server->errorMsg("Closing Link: localhost (Empty Nickname)\n<connection gets terminated by the server>\n", _userPtr->getSocket());
		return ;
	}
	if (!(_userPtr->getUsername().empty()))
	{
		msg = ERR_ALREADYREGISTRED(_userPtr->getNickname());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (content == "USER" || isOnlySpace(content) == true)
	{
		msg = ERR_NEEDMOREPARAMS(_userPtr, "USER");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	while (std::getline(iss, tmp, ' '))
		params.push_back(tmp);
	if (params.size() > 0)
		this->_userPtr->setUsername(params[0]);
	if (params.size() >= 4)
		this->_userPtr->setRealname(params[3]);
	msg = RPL_WELCOME(_userPtr);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
}
