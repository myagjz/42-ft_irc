#include "../incs/Executor.hpp"
#include "../incs/Channel.hpp"
#include <limits.h>
#include <unistd.h>

Executor::Executor(Server *ptr) {
	this->_server = ptr;
	this->_userPtr = NULL;
	this->_mapping["NICK"] = &Executor::_nick;
	this->_mapping["USER"] = &Executor::_user;
	this->_mapping["QUIT"] = &Executor::_quit;
	this->_mapping["JOIN"] = &Executor::_join;
	this->_mapping["PASS"] = &Executor::_pass;
	this->_mapping["MODE"] = &Executor::_mode;
	this->_mapping["BOT"] = &Executor::_privmsg;
	this->_mapping["privmsg"] = &Executor::_privmsg;
	this->_mapping["KICK"] = &Executor::_kick;
	this->_mapping["TOPIC"] = &Executor::_topic;
	this->_mapping["INVITE"] = &Executor::_invite;
	return ;
}

Executor::~Executor(void) {
	return ;
}

void Executor::execOPs(void) {
	size_t size = this->ops.size();
	std::string msg;
	for (size_t i = 0; i < size; i++) {
		if (_mapping.find(ops[i].type) == _mapping.end()) {
			msg = ERR_UNKNOWNCOMMAND(_userPtr, ops[i].type);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			break;
		}
		func f = _mapping[ops[i].type];
		(this->*f)(ops[i].content);	
		ops[i].type = "";
		ops[i].content = "";
	}
}

void Executor::parseBuffer(std::string content) {
	std::istringstream iss(content);
	std::string tmp;
	std::string::size_type pos = 0;

	if (!(this->ops.empty())) {
		this->ops.clear();
	}
	while (std::getline(iss, tmp, '\n')) {
		int splitter = tmp.find(' ');
		pos = tmp.find('\r');
		if (pos != std::string::npos)
			tmp = tmp.substr(0, pos);
		std::string type = tmp.substr(0, splitter);
		OP op = {
			.type = tmp.substr(0, splitter),
			.content = tmp.substr(splitter+1, tmp.size())
		};
		this->ops.push_back(op);
	}
}

void Executor::_cap(std::string content) {
	(void)(content);
	return ;
}

void Executor::_nick(std::string content) {
	std::string msg;

	if (_userPtr->checkPassword == false) {
		_pass("");
		return;
	}
	if (content.find('#') != std::string::npos || content.find(':') != std::string::npos) {
		msg =  ERR_ERRONEUSNICKNAME(content);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	for (std::list<User>::iterator ite = _server->users.begin(); ite != _server->users.end(); ite++) {
		if((*ite).getNickname() == content) {
			msg = ERR_NICKNAMEINUSE((*ite).getNickname());
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
	}
	if (content == "NICK" || isOnlySpace(content) == true) {
		msg = ERR_NONICKNAMEGIVEN();
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (!(_userPtr->getNickname().empty())) {
		msg = NICK(_userPtr->getNickname(), content);
		for (std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
			send((*it).getSocket() , msg.c_str(), msg.size(), 0);
	}
	_userPtr->setNickname(content);
}

void Executor::_quit(std::string content) {
	(void)(content);
	memset(_userPtr->buffer, 0, 4096);
	std::string msg = ":";
	msg += _userPtr->getNickname();
	msg += " QUIT :Connection closed\n";
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it).isUserByNickname(_userPtr->getNickname()) == true) {
			(*it).sendQuitReplyToAll(msg, _userPtr->getSocket());
			(*it).delUser(_userPtr);
		}
		else if ((*it).isOpByNickname(_userPtr->getNickname()) == true) {
			(*it).sendQuitReplyToAll(msg, _userPtr->getSocket());
			(*it).delUser(_userPtr);
		}
	}
	std::vector<Channel>::iterator ite = _channels.begin();
	int nbChannels = _channels.size();
	int i = 0;
    while (i < nbChannels)
	{
		for (ite = _channels.begin(); ite != _channels.end(); ite++) 
			if ((*ite).isUserLeft() == false)
			{
				i = 0;
				_channels.erase(ite);
				break;
			}
		i++;
	}
}

void Executor::_user(std::string content) {
	std::istringstream iss(content);
	std::string tmp, msg;
	std::vector<std::string> params;

	if (_userPtr->checkPassword == false) {
		_pass("");
		return;
	}
	if (_userPtr->getNickname().empty()) {
		_server->errorMsg("Closing Link: localhost (Empty Nickname)\n<connection gets terminated by the server>\n", _userPtr->getSocket());
		return ;
	}
	if (!(_userPtr->getUsername().empty())) {
		msg = ERR_ALREADYREGISTRED(_userPtr->getNickname());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (content == "USER" || isOnlySpace(content) == true) {
		msg = ERR_NEEDMOREPARAMS(_userPtr, "USER");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	while (std::getline(iss, tmp, ' '))
		params.push_back(tmp);
	if (params.size() > 0)
		this->_userPtr->setUsername(params[0]);
	if (params.size() >= 4)
		this->_userPtr->setRealname(params[3]);
	msg = RPL_WELCOME(_userPtr);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
}

void	Executor::joinChannel(std::string firstword, Channel *chanToJoin)
{
	std::string msg = RPL_JOIN(_userPtr->getNickname(), firstword);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_TOPIC(_userPtr, firstword, chanToJoin->getTopic());
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_NAMERPLY(_userPtr, (*chanToJoin), chanToJoin->getAllUsersForNameReply());
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_ENDOFNAMES(chanToJoin->getName());
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	// Resend message to all users in the channel
	char	*str;
	std::string everyone = chanToJoin->getAllUsersForNameReply();
	str = strtok(const_cast<char *>(everyone.c_str()), " ");
	while (str != NULL)
	{
		std::string name(str);
		User *tmp;
		if (str[0] == '@')
			name = name.substr(1, name.length());
		tmp = chanToJoin->getUserByNickname(name);
		msg = RPL_JOIN(_userPtr->getNickname(), firstword);
		send(tmp->getSocket(), msg.c_str(), msg.size(), 0);
		str = strtok(0, " ");
	}
	chanToJoin->addUser(_userPtr, false);
}

void Executor::_join(std::string content) {
	std::string msg;

	std::string firstword;
	std::istringstream iss(content);

	if (_userPtr->getNickname().empty() || _userPtr->getUsername().empty()) {
		std::string rep = " ";
		msg = ERR_NOTREGISTERED(rep);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	iss >> firstword;

	if (firstword.find('#') == std::string::npos || firstword == "#") {
		msg = ERR_INVALIDCHANNEL(_userPtr, firstword);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	if (!isChannel(firstword)) {
		_createChannel(firstword);
	}
	else {
		Channel *chanToJoin = getChannelByName(firstword);
		if (chanToJoin == NULL) {
			msg = ERR_NOSUCHCHANNEL(_userPtr, firstword);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		else if (chanToJoin->countUsersInChannel() >= chanToJoin->getUserLimits() && chanToJoin->getUserLimits() != 0) {
			msg = ERR_CHANNELISFULL(_userPtr, firstword);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		else if (chanToJoin->getInviteOnly() == true && _userPtr->isInvited(firstword) == false) {
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
		else if (chanToJoin->getPass() != "") {
			int end, start = 0;
			start = content.find(" ");
			start++;
			end = start;
			while (content[end] != ' ' && content[end] != '\0')
				end++; 
			std::string userPass = content.substr(start, end - start);
			if (userPass != chanToJoin->getPass()) {
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


void Executor::_pass(std::string content) {
	if(content.find(13) != std::string::npos) {
		content.erase(content.find(13));
	}
	if (_server->getPassword().compare(content) == 0) {
		_userPtr->checkPassword = true;
		return ;
	}
	else {
		std::string msg = ERR_PASSWDMISMATCH(_userPtr);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		_userPtr->passwordWasChecked = true;
		_server->errorMsg("Closing Link: localhost (Bad or empty Password)\n<connection gets terminated by the server>\n", _userPtr->getSocket());
	}
}

void Executor::_createChannel(std::string content) {
	Channel newChan(content, _userPtr);
	_channels.push_back(newChan);
	std::string msg = RPL_JOIN(_userPtr->getNickname(), content);
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_NAMERPLYCREATECHANNEL(_userPtr, newChan, newChan.getNicknameWithPrefix(_userPtr));
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	msg = RPL_ENDOFNAMES(newChan.getName());
	send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
}

bool Executor::isChannel(std::string channel)
{
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		if (channel == (*it).getName())
			return (true);
	return (false);
}


bool Executor::isHash(std::string content)
{
	if (content[0] != '#')
		return (false);
	return (true);
}

std::string	Executor::nextWord(std::string content)
{
	std::string firstword;
	std::string secword;
	std::istringstream iss(content);

	iss >> firstword >> firstword >> secword;
	if (firstword == secword)
		return ("");
	return (secword);
}

bool	Executor::isDigit(std::string content)
{
	for (int i = 0; content[i] != '\0'; i++)
		if (!isdigit(content[i]))
			return (false);
	return (true);
}

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

void Executor::_privmsg(std::string content) {
	if (content.empty()) {
		std::string msg = ERR_NEEDMOREPARAMS(_userPtr, "privmsg");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (content[0] == '#')
		_sendMessageToChan(content);
	else
		_sendPrivateMessage(content);
}

void Executor::_sendMessageToChan(std::string content) {
	std::string chanName, msg, reply;
	Channel *chan;
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	if (isChannel(chanName)) {
		chan = getChannelByName(chanName);
		if (chan->isUserAndOpByNickname(_userPtr->getNickname()) == false) {
			msg = ERR_NOTONCHANNEL(chanName);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		msg = strtok(0, "");
 		msg = msg.substr(1, msg.size());
		reply = RPL_PRIVMSG(_userPtr, chanName, msg);
		chan->sendPrivmsgReplyToChan(chan, reply, _userPtr->getSocket());
	}
	else {
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Executor::_sendPrivateMessage(std::string content) {
	std::string msg, reply, nickToSend;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	if (j == 0) {
		msg = ERR_NEEDMOREPARAMS(_userPtr, "privmsg");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	nickToSend = strtok(const_cast<char *>(content.c_str()), " ");
	if (isUserByNickname(nickToSend)) {
		msg = strtok(0, "");
		msg = msg.substr(0, msg.size());
		reply = RPL_PRIVMSG(_userPtr, nickToSend, msg);
		send(getPrivateUserByNickname(nickToSend)->getSocket(), reply.c_str(), reply.size(), 0);
		}
	else {
		msg = ERR_NOSUCHNICK(_userPtr, nickToSend);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

void Executor::_kick(std::string content) {
	std::string chanName, reason, nickToKick, msg;
	Channel *chan;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	if (j < 1) {
		msg = ERR_NEEDMOREPARAMS(_userPtr, "KICK");
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	chan = getChannelByName(chanName);
	if (isChannel(chanName) == false) {
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chan->isOp(_userPtr) == false) {
		msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	nickToKick = strtok(0, " ");
	if (chan->isOpByNickname(nickToKick) == false && chan->isUserByNickname(nickToKick) == false) {
		msg = ERR_NOSUCHNICK(_userPtr, nickToKick);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (j >= 2)
		reason = strtok(0, " ");
	chan->sendKickReplyToAll(chanName, reason, nickToKick, chan);
	chan->delUser(chan->getUserByNickname(nickToKick));
}

void Executor::_topic(std::string content) {
	std::string chanName, topic, msg;
	Channel *chan;
	int i = -1, j = 0;
	while (content[++i])
		if (content[i] == ' ')
			j++;
	chanName = strtok(const_cast<char *>(content.c_str()), " ");
	chan = getChannelByName(chanName);
	if (isChannel(chanName) == false) {
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (j == 0) {
		if (chan->getTopic() == "")
			msg = RPL_NOTOPIC(_userPtr, chanName);
		else
			msg = RPL_TOPIC(_userPtr, chanName, chan->getTopic());
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
	}
	else {
		if (chan->isOp(_userPtr) == false && chan->getTopicRestrictionForOp() == false) {
			msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
			send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
			return;
		}
		topic = strtok(0, "");
		chan->sendTopicReplyToAll(chanName, topic, chan);
		chan->setTopic(topic);
	}
}

void Executor::_invite(std::string content) {
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
	if (chanToInvite == NULL) {
		msg = ERR_NOSUCHCHANNEL(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->getInviteOnly() == 1 && chanToInvite->isOp(_userPtr) == false) {
		msg = ERR_CHANOPRIVSNEEDED(_userPtr, chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->getUser(_userPtr->getSocket()) == NULL) {
		msg = ERR_NOTONCHANNEL(chanName);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (isUserByNickname(nickToInvite) == false) {
		msg = ERR_NOSUCHNICK(_userPtr, nickToInvite);
		send(_userPtr->getSocket(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chanToInvite->isUserAndOpByNickname(nickToInvite) == true) {
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

Channel *Executor::getChannelByName(std::string channelName) {
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if (it->getName() == channelName)
			return &(*it);
	}
	return NULL;
}

bool Executor::isUser(User *user) {
		for(std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
		if (user->getNickname() == (*it).getNickname())
			return (true);
	return (false);
}

bool Executor::isUserByNickname(std::string nick) {
		for(std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++)
			if (nick == (*it).getNickname())
				return (true);
	return (false);
}

bool Executor::isOnlySpace(std::string str) {
	int i = 0;
	while (str[i]) {
		if (str[i] != ' ')
			return false;
		i++;
	}
	return true;
}

 /* GETTERS & SETTERS */
void Executor::setUserPtr(User *ptr) {
	this->_userPtr = ptr;
}

User *Executor::getUserPtr() {
	return _userPtr;
}

User *Executor::getPrivateUserByNickname(std::string nickName)
{

	for (std::list<User>::iterator it = _server->users.begin(); it != _server->users.end(); it++) {
		if ((*it).getNickname() == nickName)
			return &(*it);
	}

	return (NULL);
}