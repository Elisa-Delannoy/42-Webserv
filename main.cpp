#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include "Epoll.hpp"
#include "ParseRequest.hpp"
#include <signal.h>

int main(int argc, char **argv)
{
	HTTPServer server;

	if (argc != 2)
	{
		std::cerr << "There should be one argument (conf file)." << std::endl;
		return 1;
	}
	signal(SIGPIPE, SIG_IGN); // ignore le signal
	std::string conf_file = argv[1];
	server.startServer(conf_file);
	server.closeServer();
	return 0;
}
