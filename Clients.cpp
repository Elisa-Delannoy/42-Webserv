#include "Clients.hpp"

Clients::Clients(int fd) : _socket_fd(fd), _index_buff(0), _status(WAITING_REQUEST)
{
	this->_read_buff.clear();
	this->_write_buff.clear();
}

Clients::~Clients()
{
	close(_socket_fd);
}

Clients::status Clients::GetStatus() const
{
	return this->_status;
}

void Clients::SetStatus(status new_status)
{
	this->_status = new_status;
}

std::vector<char> Clients::GetReadBuffer()
{
	return (this->_read_buff);
}

void Clients::SetReadBUff(char c)
{
	this->_read_buff.push_back(c);
}
