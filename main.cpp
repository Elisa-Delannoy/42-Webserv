#include "HTTPServer.hpp"
#include "Epoll.hpp"
#include "ParseRequest.hpp"

int main(void)
{
	HTTPServer server;
	ParseRequest toparse(server);

	server.startServer();
	server.closeServer();
	toparse.DivideRequest();

	return 0;
}
