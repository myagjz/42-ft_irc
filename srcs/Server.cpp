#include "../incs/Server.hpp"
#include <signal.h>

bool progOver = false;

Server::Server(const char *ip, int port, int address_family, int type, std::string password)
    : _port(port), _address_family(address_family), _type(type), _password(password), serverSocket(0)
{
    FD_ZERO(&_readfds);
    _executor = new Executor(this);
	_ip = ip;
    memset(&_hint, 0, sizeof(_hint));
    memset(_host, 0, NI_MAXHOST);
    memset(_service, 0, NI_MAXSERV);
}

Server::~Server()
{
    delete _executor;
}

void Server::init() 
{
    serverSocket = socket(_address_family, _type, 0);
    if (serverSocket == -1)
        throw createSocketError();
    _handleMultiplesConnection();
}

void Server::socBind()
{
    _hint.sin_family = _address_family;
    _hint.sin_port = htons(_port);
    _hint.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (sockaddr*)&_hint, sizeof(_hint)) == -1)
        throw bindSocketError();
}

void Server::mark()
{
    if (listen(serverSocket, SOMAXCONN) < 0)
        throw markSocketError();
}

int Server::awaitForConnection()
{
    socklen_t clientsize = sizeof(_client);
    memset(_host, 0, NI_MAXHOST);
    memset(_service, 0, NI_MAXSERV);

    int clientSocket = accept(serverSocket, (sockaddr*)&_client, &clientsize);
    if (clientSocket == -1) {
        std::cerr << "Problem with the client connecting" << std::endl;
        return -4;
    }
    return clientSocket;
}

void sigHandler(int signal)
{
    if (signal == SIGINT)
        progOver = true;
}

void Server::handle()
{
    struct sigaction sigStruct;
    sigStruct.sa_handler = sigHandler;
    sigemptyset(&sigStruct.sa_mask);
    sigStruct.sa_flags = 0;
    sigaction(SIGINT, &sigStruct, NULL);

    _hintlen = sizeof(_hint);
    int valread = 0;
    char tmpBuff[4096];

    while (!progOver) 
    {
        memset(tmpBuff, 0, sizeof(tmpBuff));
        FD_ZERO(&_readfds);
        FD_SET(serverSocket, &_readfds);
        for (size_t i = 0; i < client_socket.size(); i++) {   
            if (client_socket[i] > 0)
                FD_SET(client_socket[i], &_readfds);
        }

        int activity = select(FD_SETSIZE, &_readfds, NULL, NULL, NULL);
        if (progOver) {
            for (std::vector<int>::iterator it = client_socket.begin(); it != client_socket.end(); it++)
                if(*it)
                    close(*it);
            throw sigintReceived();
        }

        if (activity < 0)
            std::cerr << "Error: select()" << std::endl; 

        _acceptIncomingConnection();

        for (size_t i = 0; i < client_socket.size(); i++) {
            _executor->setUserPtr(getUserBySocket(client_socket[i]));
            if (FD_ISSET(client_socket[i], &_readfds)) {
                if ((valread = recv(client_socket[i], tmpBuff, sizeof(tmpBuff), 0)) == 0) {
                    _handleDisconnection(i, client_socket[i]);
                    memset(tmpBuff, 0, sizeof(tmpBuff));
                } else {
                    size_t j = 0;
                    while (_executor->getUserPtr()->buffer[j] != '\0') j++;
                    strcpy(_executor->getUserPtr()->buffer + j, tmpBuff);
                    if (_executor->getUserPtr()->findNl(_executor->getUserPtr()->buffer)) {
                        std::cout << tmpBuff << std::flush;
                        _executor->parseBuffer(_executor->getUserPtr()->buffer);
                        memset(_executor->getUserPtr()->buffer, 0, sizeof(tmpBuff));
                        _executor->execOPs();
                    }
                }
            }
        }
    }
}

void Server::errorMsg(std::string reason, int clientSocket)
{
    std::string msg = "ERROR: " + reason;
    send(clientSocket, msg.c_str(), msg.size(), 0);
    cleanAnUser(clientSocket);
}

void Server::cleanAnUser(int userSocket)
{
    for (std::vector<int>::iterator it = client_socket.begin(); it != client_socket.end(); it++) {
        if ((*it) == userSocket) {
            client_socket.erase(it);
            break;
        }
    }
    for (std::list<User>::iterator it = users.begin(); it != users.end(); it++) {
        if ((*it).getSocket() == userSocket) {
            users.erase(it);
            break;
        }
    }
    close(userSocket);
}

void Server::_handleMultiplesConnection()
{
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        throw setSockOptError();
}

void Server::_acceptIncomingConnection()
{
    if (FD_ISSET(serverSocket, &_readfds)) {
        int new_socket = accept(serverSocket, (struct sockaddr *)&_hint, (socklen_t*)&_hintlen);
        if (new_socket < 0)
            throw acceptSocketError();
        
        client_socket.push_back(new_socket);
        users.push_back(User(new_socket));
    }
}

void Server::_handleDisconnection(int i, int sd)
{
    getpeername(sd, (struct sockaddr*)&_hint, (socklen_t*)&_hintlen);
    _executor->_quit("");
    getUserBySocket(sd)->checkPassword = false;
    printf("Host disconnected, ip %s, port %d \n", inet_ntoa(_hint.sin_addr), ntohs(_hint.sin_port));  
    close(sd);
    client_socket.erase(client_socket.begin() + i);
    _executor->setUserPtr(NULL);
    cleanAnUser(sd);
}

void Server::setServerSocket(int servSock) 
{
    serverSocket = servSock;
}

User *Server::getUserBySocket(int socket)
{
    for (std::list<User>::iterator it = users.begin(); it != users.end(); it++) {
        if ((*it).getSocket() == socket)
            return &(*it);
    }
    return NULL;
}

User *Server::getUserByUsername(std::string username)
{
    for (std::list<User>::iterator it = users.begin(); it != users.end(); it++) {
        if ((*it).getUsername() == username)
            return &(*it);
    }
    return NULL;
}

std::string Server::getPassword() 
{
    return _password;
}