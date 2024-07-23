#include "../incs/Executor.hpp"

void Executor::_privmsg(std::string content)
{
	if (content.empty())
	{
		std::string msg = ERR_NEEDMOREPARAMS(_userPtr, "privmsg");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (content[0] == '#')
		_sendMessageToChan(content);
	else
		_sendPrivateMessage(content);
}

void Executor::_sendMessageToChan(std::string content)
{
	std::string chanName, msg, reply;
	Channel *chan;
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	if (isChannel(chanName)) {
		chan = getChannelByName(chanName);
		if (chan->isUserAndOpByNickname(_userPtr->getNickname()) == false)
		{
			msg = ERR_NOTONCHANNEL(chanName);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		msg = strtok(0, "");
 		msg = msg.substr(1, msg.size());
		reply = RPL_PRIVMSG(_userPtr, chanName, msg);
		chan->sendPrivmsgReplyToChan(chan, reply, _userPtr->getSocket());
	}
	else
	{
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Executor::_sendPrivateMessage(std::string content)
{
	std::string msg, reply, nickToSend;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	if (j == 0)
	{
		msg = ERR_NEEDMOREPARAMS(_userPtr, "privmsg");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	nickToSend = strtok(const_cast<char *>(content.c_str()), " ");
	if (isUserByNickname(nickToSend))
	{
		msg = strtok(0, "");
		msg = msg.substr(0, msg.size());
		reply = RPL_PRIVMSG(_userPtr, nickToSend, msg);
		send(getPrivateUserByNickname(nickToSend)->getSocket(), reply.c_str(), reply.size(), 0);
	}
	else
	{
		msg = ERR_NOSUCHNICK(_userPtr, nickToSend);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
}