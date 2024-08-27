#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <cctype>
#include <signal.h>


struct addrinfo *res = NULL;
bool progOver = false;

int parsing(int ac, char **av)
{
    if (ac != 4)
    {
        std::cerr << "Error: format <port> <password> <channel>" << std::endl;
        return -1;
    }
    
    std::string port(av[1]);
    std::string password(av[2]);
    std::string channel(av[3]);

    if (port == "" || password == "" || channel == "")
    {
        std::cerr << "Error: format <port> <password> <channel>" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < port.length(); ++i)
    {
        if (!isdigit(port[i])) {
            std::cerr << "Invalid port number" << std::endl;
            return -1;
        }
    }
    
    return 0;
}

int createSocket(char **av)
{
    std::string port(av[1]);
    std::string password(av[2]);
    std::string channel(av[3]);

    struct addrinfo hints;
    int connectSocket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo("localhost", av[1], &hints, &res))
    {
        std::cerr << "Can't get addr\n";
        freeaddrinfo(res);
        return -1;
    }

    connectSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connectSocket == -1) {
        std::cerr << "Can't do socket\n";
        freeaddrinfo(res);
        return -1;
    }

    if (connect(connectSocket, res->ai_addr, (int)res->ai_addrlen) == SO_ERROR)
    {
        std::cerr << "Can't connect to server\n";
        close(connectSocket);
        freeaddrinfo(res);
        connectSocket = -1;
    }

    return connectSocket;
}


void sigHandler(int signal)
{
    if (signal == SIGINT)
        progOver = true;
}

std::string seperateUser(std::string usr)
{
	size_t pos = usr.find(':');
    if (pos != std::string::npos)
    {
        std::string result = usr.substr(pos + 1, pos + 10);
        return result;
    }
	else
		return usr;
}

void bot(int socket, char **av)
{
    struct sigaction sigStruct;
    sigStruct.sa_handler = sigHandler;
    sigemptyset(&sigStruct.sa_mask);
    sigStruct.sa_flags = 0;
    sigaction(SIGINT, &sigStruct, NULL);

    std::string password(av[2]);
    std::string channel(av[3]);
    char buff[4096];

    std::string msg = "PASS " + password + "\n";
    send(socket, msg.c_str(), msg.size(), 0);
    msg = "NICK bot\n";
    send(socket, msg.c_str(), msg.size(), 0);
    msg = "USER Bot\n";
    send(socket, msg.c_str(), msg.size(), 0);
    msg = "JOIN " + channel + "\n";
    send(socket, msg.c_str(), msg.size(), 0);
    sleep(1);

    while (!progOver)
    {
        memset(buff, 0, sizeof(buff));
        if (recv(socket, buff, sizeof(buff), 0) == 0)
        {
            msg = "QUIT\n";
            send(socket, msg.c_str(), msg.size(), 0);
            close(socket);
            freeaddrinfo(res);
            return;
        }

        if (strstr(buff, "JOIN ") != NULL)
        {
            std::istringstream iss(buff);
            std::istringstream useriss(buff);
			std::string new_user = "";
			useriss >> new_user;

            std::string word;
            while (iss >> word)
            {
				if (new_user != ":IRC")
					new_user = seperateUser(new_user);
                if (word == "JOIN")
                {
                    std::string channel(av[3]);

                    std::string privmsg = "BOT " + new_user + " :Welcome to the channel " + new_user + "! You have joined " + channel + "\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    
                    privmsg = "BOT " + new_user + " :██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    privmsg = "BOT " + new_user + " :██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    privmsg = "BOT " + new_user + " :██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    privmsg = "BOT " + new_user + " :██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    privmsg = "BOT " + new_user + " :╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                    privmsg = "BOT " + new_user + " : ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n";
                    send(socket, privmsg.c_str(), privmsg.size(), 0);
                }
            }
        }
    }
}

int main(int ac, char **av)
{
    int socket = 0;
    if (parsing(ac, av) == -1)
        return -1;

    socket = createSocket(av);
    if (socket == -1)
        return -1;

    bot(socket, av);
    return 0;
}