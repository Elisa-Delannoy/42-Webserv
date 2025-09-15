#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>

class Response
{
	public:
		Response();
		Response(int client_fd);
		~Response();

		void displayBody();
		void displayImg();

	private:
		int _client_fd;
};

#endif