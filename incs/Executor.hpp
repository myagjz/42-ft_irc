#ifndef EXECUTOR_HPP
# define EXECUTOR_HPP

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <sys/socket.h>
# include "User.hpp"
# include "ServerReply.hpp"
# include "Server.hpp"
# include "Channel.hpp"

class User;
class Server;
class Executor;
typedef void (Executor::*func)(std::string content);
typedef std::map<std::string, func> mapFunc;

struct OP
{
	std::string	type;
	std::string content;
};

class Executor
{

	private:
		void _cap(std::string content);
		void _nick(std::string content);
		void _user(std::string content);
		void _join(std::string content);
		void _pass(std::string content);
		void _mode(std::string content);
		void _privmsg(std::string content);
		void _kick(std::string content);
		void _topic(std::string content);
		void _invite(std::string content);
		void _part(std::string content);
		
		void _createChannel(std::string content);
		void _sendMessageToChan(std::string content);
		void _sendPrivateMessage(std::string content);

		mapFunc _mapping;
		Server *_server;
		User *_userPtr;
		std::vector<Channel> _channels;

	public:
		std::vector<OP>	ops;

		Executor(Server *ptr);
		~Executor(void);
		void parseBuffer(std::string content);
		void execOPs(void);
		Channel	*getChannelByName(std::string channelName);

		void _quit(std::string content);
		bool isChannel(std::string channel);
		bool isUser(User *user);
		std::string	nextWord(std::string content);
		bool	isDigit(std::string content);
		bool isUserByNickname(std::string nick);
		void	joinChannel(std::string firstword, Channel *chanToJoin);
		User *getPrivateUserByNickname(std::string nickName);
		bool isOnlySpace(std::string str);
		
		void setUserPtr(User *ptr);
		User *getUserPtr();

};

#endif