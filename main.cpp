#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include "Epoll.hpp"
#include "ParseRequest.hpp"

int main(void)
{
	HTTPServer server;

	server.startServer();
	server.closeServer();
	return 0;
}
