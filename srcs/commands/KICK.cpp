#include "../incs/Executor.hpp"

void Executor::_kick(std::string content)
{
	std::string chanName, reason, nickToKick, msg;
	Channel *chan;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	if (j < 1)
	{
		msg = ERR_NEEDMOREPARAMS(_userPtr, "KICK");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	chan = getChannelByName(chanName);
	if (isChannel(chanName) == false)
	{
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chan->isOp(_userPtr) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	nickToKick = strtok(0, " ");
	if (chan->isOpByNickname(nickToKick) == false && chan->isUserByNickname(nickToKick) == false)
	{
		msg = ERR_NOSUCHNICK(_userPtr, nickToKick);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (j >= 2)
		reason = strtok(0, " ");
	chan->sendKickReplyToAll(chanName, reason, nickToKick, chan);
	chan->delUser(chan->getUserByNickname(nickToKick));
}