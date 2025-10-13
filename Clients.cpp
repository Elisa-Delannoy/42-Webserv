#include "Clients.hpp"

Clients::Clients(int fd, int server_index) : _socket_fd(fd), _server_index(server_index),
	 _r_header(false),  _recv(0), _status(WAITING_REQUEST), _cgistatus(CGI_NONE)
{
	this->_read_buff.clear();
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

Clients::cgistatus Clients::GetCgiStatus() const
{
	return (this->_cgistatus);
}

void	Clients::SetCgiStatus(cgistatus new_status)
{
	this->_cgistatus = new_status;
}

std::vector<char>& Clients::GetReadBuffer()
{
	return (this->_read_buff);
}

int&		Clients::GetRecv()
{
	return (this->_recv);
}
void	Clients::SetRecv(int count)
{
	this->_recv = count;
}

void Clients::SetReadBuff(char* buff, size_t len)
{
	this->_read_buff.insert(this->_read_buff.end(), buff, buff + len);
}

void Clients::ClearBuff()
{
	this->_read_buff.clear();
	this->_head.SetIndexEndHeader(0);
	this->_r_header = false;
	this->_recv = 0;
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

