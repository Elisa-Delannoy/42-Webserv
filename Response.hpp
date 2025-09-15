#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <sys/stat.h>

class Response
{
	public:
		Response(int client_fd);
		~Response();

		void displayBody();
		void displayImg();

	private:
		std::string _response;
		int _client_fd;
};

#endif