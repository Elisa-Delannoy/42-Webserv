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
#include "ExecCGI.hpp"
// #include "Clients.hpp"




class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int startServer(std::string conf_file);
		int runServer();
		void closeServer();
		int prepareServerSockets();
		int createServerSocket(std::vector<std::pair<std::string, int> > &host_port, size_t i, size_t j);
		int	AcceptRequest(int used_socket, Epoll& epoll, int j);
		int	GetServerIndex(int used_socket);

		//Parsing .conf
		bool ParsingConf(std::string conf_file);
		void displayServers();
		std::vector<ServerConf> servers;

		// const char* GetRequest(void) const;

		int readHeaderRequest(int client_fd, Clients* client, std::vector<char> request);
		void handleRequest(Epoll& epoll, int i, Clients* client);

		uint32_t 	prepareAddrForHtonl(std::string addr);
		bool 		checkPortHostTaken(std::vector<std::pair<std::string, int> >host_port, std::string host, int port);
		Clients*	FindClient(int fd);
		void 		ReadAllRequest(Clients* client, int fd);
		int			CheckEndRead(Clients* client, char* buffer);
		int			CheckEndWithChunk(Clients* client, std::vector<char> buffer);
		int			CheckEndWithLen(Clients* client);

	private:

		std::vector<int> 		_socket_server;
		std::map<int, Clients*> _socket_client;
		std::map<int, size_t> 	_attached_server;
		// char*					 _body_buf;



};

#endif