#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/socket.h>
#include <sys/stat.h>
#include <algorithm>
#include "ParseRequest.hpp"

#define ROOT "html/index.html"
#define ERROR404 "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>MyWebServ</center></body></html>"
#define ERROR500 "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>MyWebServ</center></body></html>"

class Response
{
	public:
		Response(std::map<int, std::string> errors_path, int client_fd, int body_len);
		~Response();

		std::string setStatus(std::string version, std::string code);
		std::string setContentType(std::string path);
		std::string setContentLength(std::string path);
		void setHeader(std::string version, std::string path, int code);
		void sendHeader();
		void sendBody();
		void sendError(int code);
		void sendHeaderAndBody();
		void sendResponse(ParseRequest header, char* buf);

		int checkBody(const char* path);
		void sendImage(std::string path_image);

		std::string GetErrorPath(int type_error);
		

		std::string setSize(const char* path_image);

	private:
		std::string _response;
		std::string _content;
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		std::string _connection;
		std::map<int, std::string> _errors_path;
		std::string _404_path;
		std::string _500_path;
		int _client_fd;
		struct stat _info;
		int _body_len;
};

#endif