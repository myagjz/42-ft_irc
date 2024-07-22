#include "../incs/Channel.hpp"

Channel::Channel(std::string chanName, User *userPtr) {
	for (std::string::size_type i = 0; i < chanName.length(); i++)
		if (chanName[i] != '\r')
			_name += chanName[i];
	_topicRestrictionForOp = false;
	_inviteOnly = false;
	_userLimits = 0;
	_topic = "";
	_pass = "";
	addUser(userPtr, true);
}

Channel::~Channel(void) {}

void		Channel::setInviteOnly(bool b)
{
	_inviteOnly = b;
}

bool		Channel::getTopicRestrictionForOp(void)
{
	return (_topicRestrictionForOp);
}

void		Channel::setTopicRestrictionForOp(bool b)
{
	_topicRestrictionForOp = b;
}

bool		Channel::getInviteOnly(void)
{
	return (_inviteOnly);
}

void		Channel::setUserLimits(unsigned int limit)
{
	_userLimits = limit;
}

unsigned int		Channel::getUserLimits(void)
{
	return (_userLimits);
}

unsigned int	Channel::countUsersInChannel()
{
	unsigned int count = 0;
	std::vector<User *>::iterator it = _user.begin();
	while (it != _user.end())
	{
		count++;
		it++;
	}
	it = _op.begin();
	while (it != _op.end())
	{
		count++;
		it++;
	}
	return (count);
}

void		Channel::setName(std::string name)
{
	for (std::string::size_type i = 0; i < name.length(); i++)
		if (name[i] != '\r')
			_name += name[i];
}

std::string	Channel::getName(void)
{
	return (_name);
}

void		Channel::setTopic(std::string topic)
{
	_topic = topic;
}

std::string	Channel::getTopic(void)
{
	return (_topic);
}

void		Channel::setPass(std::string pass)
{
	_pass = pass;
}

std::string	Channel::getPass(void)
{
	return (_pass);
}

void		Channel::addUser(User *user, bool op)
{
	if (op == true)
		_op.push_back(user);
	else
		_user.push_back(user);
}

void	Channel::delUser(User *user)
{
	std::vector<User *>::iterator it;

	it = _op.begin();

	while (it != _op.end())
	{
		if ((*it)->getSocket() == user->getSocket())
		{
			_op.erase(it);
			break ;
		}
		it++;
	}

	it = _user.begin();
	while (it != _user.end())
	{
		if ((*it)->getSocket() == user->getSocket())
		{
			_user.erase(it);
			break ;
		}
		it++;
	}
}

User		*Channel::getUser(int sock)
{
	std::vector<User *>::iterator it;

	it = _op.begin();
	while (it != _op.end())
	{
		if ((*it)->getSocket() == sock)
			return (*it);
		it++;
	}

	it = _user.begin();
	while (it != _user.end())
	{
		if ((*it)->getSocket() == sock)
			return (*it);
		it++;
	}

	return (NULL);
}

User		*Channel::getUserByNickname(std::string nickName)
{
	std::vector<User *>::iterator it;

	it = _op.begin();
	while (it != _op.end())
	{
		if ((*it)->getNickname() == nickName)
			return (*it);
		it++;
	}

	it = _user.begin();
	while (it != _user.end())
	{
		if ((*it)->getNickname() == nickName)
			return (*it);
		it++;
	}

	return (NULL);
}

User		*Channel::getUserByUsername(std::string userName)
{
	std::vector<User *>::iterator it;

	it = _op.begin();
	while (it != _op.end())
	{
		if ((*it)->getUsername() == userName)
			return (*it);
		it++;
	}

	it = _user.begin();
	while (it != _user.end())
	{
		if ((*it)->getUsername() == userName)
			return (*it);
		it++;
	}

	return (NULL);
}

unsigned int		Channel::totalUser(void)
{
	unsigned int	nb = 0;
	std::vector<User *>::iterator it;

	it = _op.begin();
	while (it != _op.end())
	{
		it++;
		nb++;
	}

	it = _user.begin();
	while (it != _user.end())
	{
		it++;
		nb++;
	}

	return (nb);
}

bool		Channel::isOp(User *user)
{
	std::string	tmp;
	std::vector<User *>::iterator it;

	it = _user.begin();
	while (it != _user.end())
	{
		if (user->getNickname() == (*it)->getNickname())
			return (false);
		it++;
	}

	it = _op.begin();
	while (it != _op.end())
	{
		if (user->getNickname() == (*it)->getNickname())
			return (true);
		it++;
	}
	return (false);
}

std::string	Channel::getNicknameWithPrefix(User *user)
{
	std::string	tmp;
	
	if (isOp(user) == true)
	{
		tmp = "@";
		tmp += user->getNickname();
	}
	else
		tmp += user->getNickname();
	return (tmp);
}

bool		Channel::isUserByNickname(std::string nickname)
{
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++)
	{
		if (nickname == (*it)->getNickname())
			return (true);
	}
	return (false);
}

bool		Channel::isOpByNickname(std::string nickname)
{
	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++)
		if (nickname == (*it)->getNickname())
			return (true);
	return (false);
 }

bool		Channel::isUserAndOpByNickname(std::string nickname) {
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++)
		if (nickname == (*it)->getNickname())
			return (true);
	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++)
		if (nickname == (*it)->getNickname())
			return (true);
	return (false);
}

std::string		Channel::getAllUsersForNameReply()
{
	std::string names;
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++)
	{
		names += getNicknameWithPrefix(*(it));
		names += " ";
	}
	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++)
	{
		names += getNicknameWithPrefix(*(it));
		names += " ";
	}
	return names;
}

void Channel::sendKickReplyToAll(std::string chanName, std::string reason, std::string nickToKick, Channel *chan) {
	std::string msg;
	for (std::vector<User *>::iterator it = chan->_op.begin(); it != chan->_op.end(); it++) {
		msg = RPL_KICK((*it)->getNickname(), chanName, nickToKick, reason);
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	for (std::vector<User *>::iterator it = chan->_user.begin(); it != chan->_user.end(); it++) {
		msg = RPL_KICK((*it)->getNickname(), chanName, nickToKick, reason);
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Channel::sendTopicReplyToAll(std::string chanName, std::string topic, Channel *chan) {
	std::string msg;
	for (std::vector<User *>::iterator it = chan->_op.begin(); it != chan->_op.end(); it++) {
		msg = RPL_TOPIC((*it), chanName, topic);
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	for (std::vector<User *>::iterator it = chan->_user.begin(); it != chan->_user.end(); it++) {
		msg = RPL_TOPIC((*it), chanName, topic);
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
}
void Channel::sendQuitReplyToAll(std::string msg, int socketSender) {

	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++) {
		if ((*it)->getSocket() != socketSender)
			send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++) {
		if ((*it)->getSocket() != socketSender)
			send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Channel::sendModeReplyToAll(std::string msg)
{
	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++)
	{
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++)
	{
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Channel::sendPlayReplyToAll(Channel *chan, std::string msg, User *sender)
{
	std::string reply;
	for (std::vector<User *>::iterator it = chan->_op.begin(); it != chan->_op.end(); it++) {
		reply = RPL_PRIVMSG(sender, chan->getName(), msg);
		send((*it)->getSocket(), reply.c_str(), reply.size(), 0);
	}
	for (std::vector<User *>::iterator it = chan->_user.begin(); it != chan->_user.end(); it++) {
		reply = RPL_PRIVMSG(sender, chan->getName(), msg);
		send((*it)->getSocket(), reply.c_str(), reply.size(), 0);
	}
}

void Channel::sendPrivmsgReplyToChan(Channel *chan, std::string reply, int socketSender)
{
	for (std::vector<User *>::iterator it = chan->_op.begin(); it != chan->_op.end(); it++) {
		if ((*it)->getSocket() != socketSender)
			send((*it)->getSocket(), reply.c_str(), reply.size(), 0);
	}
	for (std::vector<User *>::iterator it = chan->_user.begin(); it != chan->_user.end(); it++) {
		if ((*it)->getSocket() != socketSender)
			send((*it)->getSocket(), reply.c_str(), reply.size(), 0);
	}
}

bool Channel::isUserLeft()
{
	for (std::vector<User *>::iterator it = _op.begin(); it != _op.end(); it++)
		return (true);
	for (std::vector<User *>::iterator it = _user.begin(); it != _user.end(); it++)
		return (true);
	return (false);
}

std::string	Channel::getActiveModes(void)
{
	std::string msg = "";
	if (_topicRestrictionForOp == true)
		msg += "t,";
	if (_inviteOnly == true)
		msg += "i,";
	if (_userLimits != 0)
		msg += "l,";
	if (_pass != "")
		msg += "k,";
	if (msg == "")
		return ("");
	return (msg.erase(msg.size() - 1));
}