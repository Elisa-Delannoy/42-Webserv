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
#include <vector>
#include "ServerConf.hpp"
#include "Response.hpp"

class ParseRequest;

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int startServer();
		void closeServer();
		int prepareServerSocket();

		std::vector<ServerConf> ParsingConf();
		
		// const char* GetRequest(void) const;

		void readHeaderRequest(int client_fd, ParseRequest& request);
		void handleRequest(Epoll epoll, int i);

		
		
	private:
		int _socket_server;
		int _socket_client;
		sockaddr_in _sockaddr;
		char* _body_buf;
};

#endif