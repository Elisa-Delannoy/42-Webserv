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
class BodyResponse;

#define ERROR404 "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>MyWebServ</center></body></html>"
#define ERROR500 "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>MyWebServ</center></body></html>"

class Response
{
	public:
		Response(ServerConf & servers, Clients* client);
		~Response();

		int sendResponse(ServerConf & servers, Clients* client, std::vector<char> request);
		void setRootLocation(std::string & path);
		void sendError(HeaderResponse & header, BodyResponse & body, int code);
		void sendHeaderAndBody(HeaderResponse & header, BodyResponse & body);

		void handleGet(HeaderResponse & header, BodyResponse & body, std::string & path);
		void handlePathDir(HeaderResponse & header, BodyResponse & body, std::string & path);


		std::string GetErrorPath(int type_error);
		std::string getIndex();
		bool getAutoindex();

		void displayAutoindex(HeaderResponse & header, BodyResponse & body, std::string path);
		void displayUploadSuccessfull(HeaderResponse & header, BodyResponse & body);

		//POST
		void createFileOnServer(Clients* client, HeaderResponse & header, BodyResponse & body, std::string str);

	private:
		ServerConf _server;
		std::map<int, std::string> _errors_path;
		std::vector<std::string> _methods;
		std::string _root;
		int _index_location;

	protected:
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		int _body_len;
		int _client_fd;
};

#endif