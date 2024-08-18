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

    std::vector<std::string> channels;
    std::string channel;
    std::istringstream channelStream(firstword);
    while (std::getline(channelStream, channel, ','))
    {
        channels.push_back(channel);
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        channel = channels[i];

        if ((channel.find('#') == std::string::npos && channel.find('&') == std::string::npos) || (channel == "#" || channel == "&"))
        {
            msg = ERR_INVALIDCHANNEL(_userPtr, channel);
            send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
            continue;
        }
        if (!isChannel(channel))
        {
            _createChannel(channel);
        }
        else
        {
            Channel *chanToJoin = getChannelByName(channel);
            if (chanToJoin == NULL)
            {
                msg = ERR_NOSUCHCHANNEL(_userPtr, channel);
                send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
                continue;
            }
            else if (chanToJoin->countUsersInChannel() >= chanToJoin->getUserLimits() && chanToJoin->getUserLimits() != 0)
            {
                msg = ERR_CHANNELISFULL(_userPtr, channel);
                send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
                continue;
            }
            else if (chanToJoin->getInviteOnly() == true && _userPtr->isInvited(channel) == false)
            {
                msg = ERR_INVITEONLYCHAN(_userPtr, channel);
                send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
                continue;
            }
            else if (chanToJoin->isUserAndOpByNickname(_userPtr->getNickname()))
            {
                msg = ERR_ALREADYREGISTERED(_userPtr->getNickname());
                send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
                continue;
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
                    continue;
                }
                else
                    joinChannel(channel, chanToJoin);
            }
            else
                joinChannel(channel, chanToJoin);
        }
    }
}

void Executor::joinChannel(std::string channel, Channel *chanToJoin)
{
    std::string msg = RPL_JOIN(_userPtr->getNickname(), channel);
    send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
    msg = RPL_TOPIC(_userPtr, channel, chanToJoin->getTopic());
    send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
    msg = RPL_NAMERPLY(_userPtr, (*chanToJoin), chanToJoin->getAllUsersForNameReply());
    send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
    msg = RPL_ENDOFNAMES(chanToJoin->getName());
    send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);

    char *str;
    std::string everyone = chanToJoin->getAllUsersForNameReply();
    str = strtok(const_cast<char *>(everyone.c_str()), " ");
    while (str != NULL)
    {
        std::string name(str);
        User *tmp;
        if (str[0] == '@')
            name = name.substr(1, name.length());
        tmp = chanToJoin->getUserByNickname(name);
        msg = RPL_JOIN(_userPtr->getNickname(), channel);
        send(tmp->getSocket(), msg.c_str(), msg.size(), 0);
        str = strtok(0, " ");
    }
    chanToJoin->addUser(_userPtr, false);
}
