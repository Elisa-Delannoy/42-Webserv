#include "Clients.hpp"

Clients::Clients()
{
}

Clients::Clients(SocketServer socker_server, int socket_fd) : _recv(0), _socket_fd(socket_fd),
	 _r_header(false), _socket_server(socker_server), _status(WAITING_REQUEST), _cgistatus(CGI_NONE)
{
	this->_read_buff.clear();
	this->_head.SetIndexEndHeader(0);
	this->SetLastActivity();
}

Clients::~Clients()
{
	close(_socket_fd);
}

int		Clients::GetSessionId() const
{
	return (this->_session_id);
}

int		Clients::GetLastActivity() const
{
	return (this->_last_activity);
}

int		Clients::GetBeginRequest() const
{
	return (this->_t_begin_request);
}

SocketServer Clients::GetSocketServer() const
{
	return _socket_server;
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

int&	Clients::GetRecv()
{
	return (this->_recv);
}

void	Clients::SetSessionId(int id)
{
	this->_session_id = id;
}

void	Clients::SetLastActivity()
{
	time_t now = time(NULL);
	this->_last_activity = now;
}

void	Clients::SetBeginRequest()
{
	time_t now = time(NULL);
	this->_t_begin_request = now;
}

void	Clients::SetRecv(int count)
{
	this->_recv = count;
}

void Clients::SetReadBuff(char* buff, ssize_t len)
{
	if (len > 0)
		this->_read_buff.insert(this->_read_buff.end(), buff, buff + len);
}

void Clients::ClearBuff()
{
	this->_read_buff.clear();
	this->_r_header = false;
	this->_recv = 0;
	this->_t_begin_request = 0;
	this->_head.ClearHeader();
	this->_body.ClearBody();
}

int Clients::GetServerIndex() const
{
	return (this->_socket_server.GetServerIndex());
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

