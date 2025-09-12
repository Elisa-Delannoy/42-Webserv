#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int startServer();
		void closeServer();
		int prepareServerSocket();
		
		const char* GetRequest(void) const;
		
		
	private:
		int socket_server;
		int socket_client;
		sockaddr_in sockaddr;
		socklen_t len;
		char buffer_request[1024];
};

#endif