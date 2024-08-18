#include "../incs/Executor.hpp"

void Executor::_part(std::string content)
{
    std::string chanName, reason, msg;
    Channel *chan;
    int i = -1, j = 0;

    while (content[++i])
        if (content[i] == ' ')
            j++;
    if (j < 1)
    {
        msg = ERR_NEEDMOREPARAMS(_userPtr, "PART");
        send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
        return;
    }

    chanName = strtok(const_cast<char *>(content.c_str()), " ");
    chan = getChannelByName(chanName);

    if (chan == NULL)
    {
        msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
        send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
        return;
    }

    if (!chan->countUsersInChannel())
    {
        msg = ERR_NOTONCHANNEL(_userPtr, chanName);
        send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
        return;
    }

    if (j >= 1)
        reason = strtok(NULL, " ");
    if (reason.size() > 0 && reason[0] == ' ')
        reason.erase(0, 1);

    chan->isUserLeft();

    msg = ":" + _userPtr->getNickname() + " PART " + chanName;
    if (!reason.empty())
        msg += " :" + reason;
    msg += "\r\n";
    chan->sendPartReplyToAll(msg);

    msg = RPL_PART(_userPtr->getNickname(), chanName);
    send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);

    if (chan->totalUser() == 0)
    {
        std::vector<Channel>::iterator it = _channels.begin();
        while (it != _channels.end())
        {
            if (it->getName() == chanName)
            {
                it = _channels.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
