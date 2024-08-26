#include "../incs/Executor.hpp"

void Executor::_mode(std::string content)
{
	std::string	arg;
	std::string msg;
	std::string channel;
	std::istringstream iss(content);
	iss >> channel >> arg;
	size_t		pos = 0;
	pos = content.find(' ');
	std::vector<Channel>::iterator it = _channels.begin();

	std::string	firstWord = content.substr(0, pos);
	pos = content.find(' ');
	while (it != _channels.end())
	{
		if ((*it).getName() == firstWord)
			break;
		it++;
	}
	if (channel == "")
	{
		msg = ERR_NEEDMOREPARAMS(_userPtr, "MODE");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else if (it == _channels.end())
	{
		msg = ERR_NOSUCHCHANNEL(_userPtr, channel);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	else if (arg.empty())
	{
		msg = CHANMODES((*it).getActiveModes());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else if (content == (*it).getName())
	{
		msg = RPL_CHANNELMODEIS(_userPtr, (*it).getName(), "");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else if ((*it).isOp(_userPtr) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(_userPtr, (*it).getName());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else
	{
		if (channel != arg)
		{
			if (arg == "+i")
				(*it).setInviteOnly(true);
			else if (arg == "-i")
				(*it).setInviteOnly(false);
			else if (arg == "+t")
				(*it).setTopicRestrictionForOp(true);
			else if (arg == "-t")
				(*it).setTopicRestrictionForOp(false);
			else if (arg == "-k")
				(*it).setPass("");
			else if (arg == "-l")
				(*it).setUserLimits(0);
			else if (nextWord(content) == "")
			{
				msg = ERR_NEEDMOREPARAMS(_userPtr, arg);
				send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			}
			else if (arg == "-o")
			{
				User	*tmp = (*it).getUserByNickname(nextWord(content));
				if (tmp == NULL)
				{

					msg = ERR_NOSUCHNICK(_userPtr, nextWord(content));
					send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
					return ;
				}
				else if ((*it).isOp(tmp))
				{
					(*it).delUser(tmp);
					(*it).addUser(tmp, false);
					msg = RPL_MODE(_userPtr, (*it).getName(), "-o", nextWord(content));
					(*it).sendModeReplyToAll(msg);
				}
				else if ((*it).isUserByNickname(tmp->getNickname()))
				{
					msg = ERR_NOSUCHNICK(_userPtr, tmp->getNickname());
					send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
				}
				else
				{
					msg = ERR_USERNOTINCHANNEL(_userPtr, nextWord(content), (*it).getName());
					send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
				}
			}
			else if (arg != "+o" && arg != "+l" && arg != "+k")
			{
				msg = ERR_UNKNOWNMODE(_userPtr, (*it).getName(), nextWord(content));
				send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			}
			else if (arg == "+o")
			{
				std::string user = nextWord(content);
				if ((*it).isUserByNickname(user))
				{
					User	*tmp = (*it).getUserByNickname(user);
					(*it).delUser(tmp);
					(*it).addUser(tmp, true);
					msg = RPL_MODE(_userPtr, (*it).getName(), "+o", user);
					(*it).sendModeReplyToAll(msg);
				}
				else
				{
					msg = ERR_USERNOTINCHANNEL(_userPtr, user, (*it).getName());
					send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
				}
			}
			else if (arg == "+l")
			{
				if (isDigit(nextWord(content)))
					(*it).setUserLimits(static_cast<unsigned int>(std::atoi(nextWord(content).c_str())));
				else
				{
					msg = ERR_UNKNOWNMODE(_userPtr, (*it).getName(), nextWord(content));
					send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
				}
			}
			else if (arg == "+k")
				(*it).setPass(nextWord(content));
			else
			{
				msg = ERR_UNKNOWNMODE(_userPtr, (*it).getName(), nextWord(content));
				send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			}
		}
	}
}