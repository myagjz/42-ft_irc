#include "../incs/Server.hpp"
#include <signal.h>

int main(int ac, char **av) {
	if (ac != 3 || av[1][0] == '\0' || av[2][0] == '\0' || !(isdigit(av[1][0])) || std::strlen(av[1]) > 5) {
		std::cerr << "USAGE : ./ircserv <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (atoi(av[1]) > 65535 || atoi(av[1]) == 0) {
		std::cerr << "USAGE : ./ircserv <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
	Server *master_socket = new Server("0.0.0.0", atoi(av[1]), AF_INET, SOCK_STREAM, av[2]);
	try {
		master_socket->init();
		master_socket->socBind();
		master_socket->mark();
		master_socket->handle();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		close(master_socket->serverSocket);
	}

	delete master_socket;
	return 0;
}