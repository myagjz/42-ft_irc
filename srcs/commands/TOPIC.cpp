#include "../incs/Executor.hpp"

void Executor::_topic(std::string content)
{
	std::string chanName, topic, msg;
	Channel *chan;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	chan = getChannelByName(chanName);
	if (isChannel(chanName) == false)
	{
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (j == 0)
	{
		if (chan->getTopic() == "")
			msg = RPL_NOTOPIC(_userPtr, chanName);
		else
			msg = RPL_TOPIC(_userPtr, chanName, chan->getTopic());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else
	{
		if (chan->isOp(_userPtr) == false && chan->getTopicRestrictionForOp() == false)
		{
			msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		topic = strtok(0, "");
		chan->sendTopicReplyToAll(chanName, topic, chan);
		chan->setTopic(topic);
	}
}