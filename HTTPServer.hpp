#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int startServer();
		void closeServer();
		int prepareServerSocket();

	private:
		int socket_server;
		int socket_client;
		int epoll_fd;
		epoll_event events[10];
		epoll_event client_event;
		epoll_event server_event;
		sockaddr_in sockaddr;
};

#endif