#include "../incs/Executor.hpp"

void Executor::_quit(std::string content)
{
	(void)(content);
	memset(_userPtr->buffer, 0, 4096);
	std::string msg = ":";
	msg += _userPtr->getNickname();
	msg += " QUIT :Connection closed\n";
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if ((*it).isUserByNickname(_userPtr->getNickname()) == true)
		{
			(*it).sendQuitReplyToAll(msg, _userPtr->getSocket());
			(*it).delUser(_userPtr);
		}
		else if ((*it).isOpByNickname(_userPtr->getNickname()) == true)
		{
			(*it).sendQuitReplyToAll(msg, _userPtr->getSocket());
			(*it).delUser(_userPtr);
		}
	}
	std::vector<Channel>::iterator ite = _channels.begin();
	int nbChannels = _channels.size();
	int i = 0;
    while (i < nbChannels)
	{
		for (ite = _channels.begin(); ite != _channels.end(); ite++) 
			if ((*ite).isUserLeft() == false)
			{
				i = 0;
				_channels.erase(ite);
				break;
			}
		i++;
	}
}