#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include "ParseRequest.hpp"
#include <vector>
#include "ServerConf.hpp"
#include "Clients.hpp"

class HeaderResponse;

#define ERROR404 "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>MyWebServ</center></body></html>"
#define ERROR500 "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>MyWebServ</center></body></html>"

class Response
{
	public:
		Response(ServerConf & servers, Clients* client);
		~Response();

		void sendBody();
		void sendError(HeaderResponse & header, int code);
		void sendHeaderAndBody(HeaderResponse & header);
		void sendResponse(ServerConf & servers, Clients* client, std::vector<char> buf);
		void handleGet(HeaderResponse & header, std::string & path);
		void handlePathDir(HeaderResponse & header, std::string & path);

		int checkBody(const char* path);

		std::string GetErrorPath(int type_error);
		void setRootLocation(std::string & path);
		std::string getIndex();
		bool getAutoindex();
		void displayAutoindex(HeaderResponse & header, std::string path);
		void displayUploadSuccessfull(HeaderResponse & header);

	private:
		ServerConf _server;
		std::map<int, std::string> _errors_path;
		std::string _root;
		std::string _content;
		struct stat _info;
		int _index_location;

	protected:
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		int _body_len;
		int _client_fd;
};

#endif