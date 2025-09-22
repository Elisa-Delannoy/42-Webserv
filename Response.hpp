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
		Response(int client_fd);
		~Response();

		void setStatus(std::string version);
		void setContentType(std::string path);
		void setContentLength(std::string path);
		void sendHeaders(ParseRequest request);
		void sendContent(ParseRequest request, char* buf, int size);

	private:
		std::string _response;
		std::string _content;
		std::string _status;
		std::string _content_type;
		std::string _content_length;
		int _client_fd;
		struct stat _info;
};

#endif