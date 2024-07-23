#include "../incs/Executor.hpp"

void Executor::_join(std::string content)
{
	std::string msg;

	std::string firstword;
	std::istringstream iss(content);

	if (_userPtr->getNickname().empty() || _userPtr->getUsername().empty())
	{
		std::string rep = " ";
		msg = ERR_NOTREGISTERED(rep);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	iss >> firstword;

	if (firstword.find('#') == std::string::npos || firstword == "#")
	{
		msg = ERR_INVALIDCHANNEL(_userPtr, firstword);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (!isChannel(firstword))
	{
		_createChannel(firstword);
	}
	else
	{
		Channel *chanToJoin = getChannelByName(firstword);
		if (chanToJoin == NULL)
        {
			msg = ERR_NOSUCHCHANNEL(_userPtr, firstword);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		else if (chanToJoin->countUsersInChannel() >= chanToJoin->getUserLimits() && chanToJoin->getUserLimits() != 0)
		{
			msg = ERR_CHANNELISFULL(_userPtr, firstword);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		else if (chanToJoin->getInviteOnly() == true && _userPtr->isInvited(firstword) == false) 
        {
			msg = ERR_INVITEONLYCHAN(_userPtr, firstword);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		else if (chanToJoin->isUserAndOpByNickname(_userPtr->getNickname()))
		{
			msg = ERR_ALREADYREGISTERED(_userPtr->getNickname());
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return ;
		}
		else if (chanToJoin->getPass() != "")
		{
			int end, start = 0;
			start = content.find(" ");
			start++;
			end = start;
			while (content[end] != ' ' && content[end] != '\0')
				end++; 
			std::string userPass = content.substr(start, end - start);
			if (userPass != chanToJoin->getPass())
			{
				msg = ERR_BADCHANNELKEY(_userPtr, content);
				send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
				return;
			}
			else
				joinChannel(firstword, chanToJoin);	
		}
		else
			joinChannel(firstword, chanToJoin);	
	}
}