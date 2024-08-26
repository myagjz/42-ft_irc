#include "../incs/Executor.hpp"

void Executor::_quit(std::string content)
{
    (void)(content);
    memset(_userPtr->buffer, 0, 4096);
    
    std::string msg = ":" + _userPtr->getNickname() + " QUIT :Connection closed\n";
    
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if ((*it).isUserByNickname(_userPtr->getNickname()) || (*it).isOpByNickname(_userPtr->getNickname()))
        {
            (*it).sendQuitReplyToAll(msg, _userPtr->getSocket());
            (*it).delUser(_userPtr);
        }
    }
	
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); )
    {
        if (!(*it).isUserLeft())
            it = _channels.erase(it);
        else
            ++it;
    }
}
