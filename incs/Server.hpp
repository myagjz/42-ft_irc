#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <string>
# include <arpa/inet.h>
# include <cstring>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <map>
# include <list>
# include <vector>
# include "User.hpp"
# include "Executor.hpp"

class Executor;
class Server {

	private:
		void	_handleMultiplesConnection();
		void	_acceptIncomingConnection();
		void	_handleDisconnection(int i, int sd);

		fd_set				_readfds;
		int					_hintlen;
		Executor			*_executor;
		int					_port;
		int					_address_family;
		int					_type;
		char				_host[NI_MAXHOST];
		char				_service[NI_MAXSERV];
		const char			*_ip;
		std::string			_password;
		sockaddr_in 		_hint;
		sockaddr_in 		_client;
		

	public:
	
		Server(const char *ip, int port, int address_family, int type, std::string password);
		~Server();
		void		init();
		void		mark();
		int			awaitForConnection();
		void		socBind();
		void		handle();
		void		errorMsg(std::string reason, int clientSocket);
		void		cleanAnUser(int userSocket);

		int		serverSocket;
		std::vector<int>	client_socket;
		std::list<User>	users;
		
		/* SETTERS & GETTERS */
		void		setServerSocket(int servSock);
		User		*getUserBySocket(int socket);
		User		*getUserByUsername(std::string username);
		std::string	getPassword();


		
		/* ERROR CLASSES*/

		class bindSocketError : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Error: Can't bind socket to IP/port.");
				}
		};

		class createSocketError : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Error: Can't create a socket");
				}
		};

		class markSocketError : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Error: Can't set the socket on listening");
				}
		};

		class setSockOptError : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Error: setsockopt()");
				}
		};

		class acceptSocketError : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Error: accept()");
				}
		};
		class sigintReceived : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("\rProgram terminated");
				}
		};
};

#endif