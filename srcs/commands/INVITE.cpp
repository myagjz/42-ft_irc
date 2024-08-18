#include "../incs/Executor.hpp"

void Executor::_invite(std::string content) 
{
	std::string msg, nickToInvite, chanName;
	Channel *chanToInvite;
	int i = -1, j = 0;

	while (content[++i])
		if (content[i] == ' ')
			j++;
	if (j == 0) {
		msg = ERR_NEEDMOREPARAMS(_userPtr, "INVITE");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	nickToInvite = strtok(const_cast<char *>(content.c_str()), " ");
	chanName = strtok(0, " ");
	chanToInvite = getChannelByName(chanName);
	if (chanToInvite == NULL)
	{
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->getInviteOnly() == 1 && chanToInvite->isOp(_userPtr) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->getUser(_userPtr->getSocket()) == NULL)
	{
		msg = ERR_NOTONCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (isUserByNickname(nickToInvite) == false)
	{
		msg = ERR_NOSUCHNICK(_userPtr, nickToInvite);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->isUserAndOpByNickname(nickToInvite) == true)
	{
		msg = ERR_USERONCHANNEL(nickToInvite, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	User *userToInvite = getPrivateUserByNickname(nickToInvite);
	userToInvite->invitationList.push_back(chanName);
	msg = RPL_INVITING(chanName, nickToInvite);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_INVITE_RCV(_userPtr, chanName, nickToInvite);
	send(userToInvite->getSocket(), msg.c_str(), msg.size(), 0);
}