#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>
# include <iostream>
# include <cstring>


class User {

	private :
		int			_socket;
		std::string	_nickname;
		std::string	_username;
		std::string _realname;

		User();

	public :
		User(int socket);
		User(const User &cpy);
		User &operator=(const User &rhs);
		~User();

		bool findNl(char *str);
		bool isInvited(std::string channelName);

		char buffer[4096];
		bool checkPassword;
		bool passwordWasChecked;
		std::vector<std::string> invitationList;

		/* FUNC */
	
		/* GETTERS */
		int getSocket();
		std::string getNickname();
		std::string getUsername();
		std::string getRealname();

		/* SETTERS */
		void setSocket(int socket);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setRealname(std::string realname);
};
#endif