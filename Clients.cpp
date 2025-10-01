#include "Clients.hpp"

Clients::Clients(int fd, int server_index) : _socket_fd(fd), _server_index(server_index),
	 _count_loop_read(0), _status(WAITING_REQUEST)
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

void Clients::SetReadBuff(char* buff, size_t len)
{
	this->_read_buff.insert(this->_read_buff.end(), buff, buff + len);
	this->_count_loop_read++;
}

void Clients::ClearBuff()
{
	this->_read_buff.clear();
	this->_write_buff.clear();
}

int Clients::GetServerIndex() const
{
	return (this->_server_index);
}

int Clients::GetSocket() const
{
	return (this->_socket_fd);
}

size_t Clients::GetLoopRead() const
{
	return (this->_count_loop_read);
}