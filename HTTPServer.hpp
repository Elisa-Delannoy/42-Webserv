#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Epoll.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int startServer();
		void closeServer();
		int prepareServerSocket();

	private:
		int _socket_server;
		int _socket_client;
		sockaddr_in _sockaddr;
};

#endif