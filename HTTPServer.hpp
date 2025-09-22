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

		void readHeaderRequest(int client_fd, std::string & header);
		void getHeaderRequest(int client_fd);
		void handleRequest(Epoll epoll, int i);

		
		
	private:
		int _socket_server;
		int _socket_client;
		sockaddr_in _sockaddr;
		char* _header_buf;
		char* _body_buf;
		int _size_header_buf;
		int _size_body_buf;
};

#endif