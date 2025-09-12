#include "HTTPServer.hpp"
#include "Epoll.hpp"

int main(void)
{
	HTTPServer server;

	server.startServer();
	server.closeServer();

	return 0;
}
