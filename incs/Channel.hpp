#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "User.hpp"
# include <sys/types.h>
# include <sys/socket.h>
# include "ServerReply.hpp"

class	Channel
{
	private:
		bool					_topicRestrictionForOp;
		bool					_inviteOnly;
		size_t					_userLimits;
		std::string				_topic;
		std::string				_name;
		std::string				_pass;
		std::vector<User *>		_user;
		std::vector<User * >	_op;

	public:
		Channel(std::string chanName, User *userPtr);
		~Channel(void);

		bool			getTopicRestrictionForOp(void);
		void			setTopicRestrictionForOp(bool b);

		bool			getInviteOnly(void);
		void			setInviteOnly(bool b);

		unsigned int	getUserLimits(void);
		void			setUserLimits(unsigned int limit);
		
		unsigned int	countUsersInChannel();

		std::string		getName(void);
		void			setName(std::string name);

		std::string		getTopic(void);
		void			setTopic(std::string topic);

		std::string		getPass(void);
		void			setPass(std::string pass);

		User			*getUser(int sock);
		void			addUser(User *user, bool op);
		void			delUser(User *user);
		User			*getUserByNickname(std::string nickName);
		User			*getUserByUsername(std::string userName);
		unsigned int	totalUser(void);
		std::string		getAllUsersForNameReply();
		void			sendKickReplyToAll(std::string chanName, std::string reason, std::string nickToKick, Channel *chan);
		void			sendTopicReplyToAll(std::string chanName, std::string topic, Channel *chan);
		void			sendQuitReplyToAll(std::string msg, int socketSender);
		void			sendModeReplyToAll(std::string msg);
		void			sendPlayReplyToAll(Channel *chan, std::string msg, User *sender);
		void			sendPrivmsgReplyToChan(Channel *chan, std::string reply, int socketSender);

		std::string		getNicknameWithPrefix(User *user);
		bool			isOp(User *user);
		bool			isUserByNickname(std::string nickname);
		bool			isOpByNickname(std::string nickname);
		bool			isUserAndOpByNickname(std::string nickname);
		bool			isUserLeft();
		std::string		getActiveModes(void);

};

#endif