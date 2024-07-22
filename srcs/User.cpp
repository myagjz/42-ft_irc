#include "../incs/User.hpp"

User::User() {
}

User::User(int socket) : _socket(socket) {
	memset(this->buffer, 0, 4096);
	checkPassword = false;
	passwordWasChecked = false;
	_nickname = "";
	_username = "";
	_realname = "";
}

User::User(const User &cpy) {
	*this = cpy;
}

User &User::operator=(const User &rhs) {
	if (this != &rhs) {
		_socket = rhs._socket;
		_nickname = rhs._nickname;
		_username = rhs._username;
		_realname = rhs._realname;
	}
	return *this;
}

User::~User() {
}

/* FUNC */

bool User::findNl(char *str) {
	for(int j = 0; str[j] != '\0'; j++)
		if (str[j] == '\n')
			return true;
	return false;
}

bool User::isInvited(std::string channelName) {
	for (std::vector<std::string>::iterator it = invitationList.begin(); it != invitationList.end(); it++) {
		if (channelName == *it)
			return true;
	}
	return false;
}

/* GETTERS */

int User::getSocket() {
	return _socket;
}

std::string User::getNickname() {
	return _nickname;
}

std::string User::getUsername() {
	return _username;
}

std::string User::getRealname() {
	return _realname;
}

/* SETTERS */

void User::setSocket(int socket) {
	_socket = socket;
}

void User::setNickname(std::string nickname) {
	size_t	pos = nickname.find('\r');

	_nickname = nickname.substr(0, pos);
}

void User::setUsername(std::string username) {
	_username = username;
}

void User::setRealname(std::string realname) {
	_realname = realname;
}