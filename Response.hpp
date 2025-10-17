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

#define ERROR400 "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR404 "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR408 "<html><head><title>408 Request Timeout</title></head><body><center><h1>408 Request Timeout</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR413 "<html><head><title>413 Payload Too Large</title></head><body><center><h1>413 Payload Too Large</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR500 "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR503 "<html><head><title>503 Service Unavailable Error</title></head><body><center><h1>503 Service Unavailable Error</h1></center><hr><center>CookieServ</center></body></html>"
#define ERROR504 "<html><head><title>504 Gateway Timeout</title></head><body><center><h1>504 Gateway Timeout</h1></center><hr><center>CookieServ</center></body></html>"

class Response
{
	public:
		Response(ServerConf & servers, Clients* client);
		~Response();

		int sendResponse(ServerConf & servers, Clients* client, std::vector<char> request);
		void setRootLocationAndMethods(std::string & path);
		void sendError(HeaderResponse & header, BodyResponse & body, int code);
		void sendHeaderAndBody(HeaderResponse & header, BodyResponse & body);

		void handleCgi(HeaderResponse & header, BodyResponse & body, Clients* client);
		void handleGet(HeaderResponse & header, BodyResponse & body, std::string & path);
		void handlePost(HeaderResponse & header, BodyResponse & body, Clients* client, std::vector<char> request);
		void handleDelete(HeaderResponse & header, BodyResponse & body, std::string & path);
		void handlePathDir(HeaderResponse & header, BodyResponse & body, std::string & path);

		std::string GetErrorPath(int type_error);
		std::string getIndex();
		bool getAutoindex();
		bool isMethodAllowed(std::string method);

		void displayAutoindex(HeaderResponse & header, BodyResponse & body, std::string path);
		void displayUploadSuccessfull(HeaderResponse & header, BodyResponse & body);
		void createFileOnServer(HeaderResponse & header, BodyResponse & body, std::vector<char> & request);

	private:
		ServerConf _server;
		std::map<int, std::string> _errors_path;
		std::vector<std::string> _methods;
		std::string _root;
		int _index_location;
		bool _to_close;
		std::string _begin_405;
		std::string _end_405;
		std::string _path_unchanged;
		std::string _redirection;

	protected:
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		int _body_len;
		int _client_fd;
};

#endif