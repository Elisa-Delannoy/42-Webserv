#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <algorithm>
#include "ParseRequest.hpp"

class Response
{
	public:
		Response(int client_fd, int body_len);
		~Response();

		std::string setStatus(std::string version);
		std::string setContentType(std::string path);
		std::string setContentLength(std::string path);
		void sendHeaders(ParseRequest header);
		void sendBody(ParseRequest request, char* buf);
		void sendImage(std::string path_image);

		std::string setSize(const char* path_image);

	private:
		std::string _response;
		std::string _content;
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		int _client_fd;
		struct stat _info;
		int _body_len;
};

#endif