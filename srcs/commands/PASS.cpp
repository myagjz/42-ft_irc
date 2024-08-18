#include "../incs/Executor.hpp"

void Executor::_pass(std::string content)
{
	if(content.find(13) != std::string::npos)
	{
		content.erase(content.find(13));
	}
	if (_server->getPassword().compare(content) == 0)
	{
		_userPtr->checkPassword = true;
		return ;
	}
	else
	{
		std::string msg = ERR_PASSWDMISMATCH(_userPtr);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		_userPtr->passwordWasChecked = true;
		_server->errorMsg("Closing Link: localhost (Bad or empty Password)\n<connection gets terminated by the server>\n", _userPtr->getSocket());
	}
}