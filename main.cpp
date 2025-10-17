#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include <signal.h>

int main(int argc, char **argv)
{
	HTTPServer server;

	if (argc > 2)
	{
		std::cerr << "There should be one argument (conf file)" << std::endl;
		return 1;
	}
	std::string conf_file;
	if (argc == 1)
		conf_file = "conf/valid.conf";
	else
		conf_file = argv[1];
	signal(SIGPIPE, SIG_IGN);
	server.startServer(conf_file);
	return 0;
}
