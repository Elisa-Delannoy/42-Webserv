#ifndef CLIENTS_HPP
#define CLEINTS_HPP

#include <vector>
#include <unistd.h>
#include <ParseBodyRequest.hpp>
#include <ParseRequest.hpp>



class Clients
{
	private:
		
		int					_socket_fd;
		std::vector<char>	_read_buff;
		std::vector<char>	_write_buff;
		size_t				_index_buff;

	public:
		enum status
		{
			WAITING_REQUEST,
			PARSING_REQUEST,
			SENDING_RESPONSE,
			CLOSED
		} _status;

		Clients(int fd);
		~Clients();
		status GetStatus() const;
		void SetStatus(status new_status);
		std::vector<char> GetReadBuffer();
		void SetReadBUff(char c);


		ParseRequest		_head;
		ParseBody			_body;
};

#endif