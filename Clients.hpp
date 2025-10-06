#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include <vector>
#include <unistd.h>
#include "ParseBodyRequest.hpp"
#include "ParseRequest.hpp"


class Clients
{
	private:
		
		int					_socket_fd;
		int					_server_index;
		std::vector<char>	_read_buff;
		// std::vector<char>	_write_buff;
		bool				_r_header;

	public:
		enum status
		{
			WAITING_REQUEST,
			PARSING_REQUEST,
			SENDING_RESPONSE,
			CLOSED
		} _status;

		Clients(int fd, int server_index);
		~Clients();
		status GetStatus() const;
		void SetStatus(status new_status);
		std::vector<char> GetReadBuffer();
		void	SetReadBuff(char* c, size_t len);
		void	ClearBuff();
		int		GetServerIndex() const;
		int		GetSocket() const;
		bool	GetReadHeader() const;
		void	SetReadHeader(bool r_header);

		ParseRequest		_head;
		ParseBody			_body;
};

#endif