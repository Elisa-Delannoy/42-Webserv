#include "Clients.hpp"

Clients::Clients(int fd, int server_index) : _socket_fd(fd), _server_index(server_index),
	 _r_header(false), _status(WAITING_REQUEST)
{
	this->_read_buff.clear();
	// this->_write_buff.clear();
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

void Clients::SetReadBuff(char* buff, size_t len)
{
	this->_read_buff.insert(this->_read_buff.end(), buff, buff + len);
}

void Clients::ClearBuff()
{
	this->_read_buff.clear();
	// this->_write_buff.clear();
	this->_r_header = false;
}

int Clients::GetServerIndex() const
{
	return (this->_server_index);
}

int Clients::GetSocket() const
{
	return (this->_socket_fd);
}

bool Clients::GetReadHeader() const
{
	return (this->_r_header);
}

void Clients::SetReadHeader(bool r_header)
{
	this->_r_header = r_header;
}

