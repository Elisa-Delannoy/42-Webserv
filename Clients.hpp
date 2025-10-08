#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include <vector>
#include <unistd.h>
#include "ParseBody.hpp"
#include "ParseRequest.hpp"


class Clients
{
	private:
		
		int					_socket_fd;
		int					_server_index;
		std::vector<char>	_read_buff;
		bool				_r_header;
		int					_recv;

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

		int		GetServerIndex() const;
		int		GetSocket() const;
		int&	GetRecv();
		bool	GetReadHeader() const;
		status	GetStatus() const;
		std::vector<char>&	GetReadBuffer();

		void	SetStatus(status new_status);
		void	SetReadBuff(char* c, size_t len);
		void	SetReadHeader(bool r_header);
		void	SetRecv(int count);

		void	ClearBuff();		

		ParseRequest	_head;
		ParseBody		_body;
};

#endif