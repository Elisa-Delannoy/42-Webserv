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

		int startServer(std::string conf_file);
		void closeServer();
		int prepareServerSockets();
		int createServerSocket(std::vector<std::pair<std::string, int> > &host_port, size_t i, size_t j);

		std::vector<ServerConf> ParsingConf(std::string conf_file);
		void displayServers();
		std::vector<ServerConf> servers;
		
		// const char* GetRequest(void) const;

		void readHeaderRequest(int client_fd, ParseRequest& request);
		void handleRequest(Epoll epoll, int i);

		uint32_t prepareAddrForHtonl(std::string addr);
		bool checkPortHostTaken(std::vector<std::pair<std::string, int> >host_port, std::string host, int port);

	private:
		std::vector<ServerConf> servers;
		std::vector<int> _socket_server;
		int _socket_client;
		char* _body_buf;
};

#endif