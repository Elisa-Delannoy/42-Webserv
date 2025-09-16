#include "Response.hpp"

Response::Response(int client_fd) : _client_fd(client_fd)
{ }

Response::~Response()
{ }

void Response::setStatus(std::string version)
{
	this->_status = version + " 200 OK\r\n";
}

void Response::setContentType(std::string path)
{
	if (path == "/")
	{
		this->_content_type = "Content-Type: text/html\r\n";
	}
	else
	{
		this->_content_type = "Content-Type: image/webp\r\n";
	}
}

void Response::setContentLength(std::string path)
{
	std::ostringstream oss;
	if (path == "/")
	{
		if (stat("index.html", &this->_info) < 0)
		{
			std::cerr << "Error stat file" << std::endl;
		}
		oss << this->_info.st_size;
		std::string size = oss.str();
		this->_content_length = "Content-Length: " + size + "\r\n";
	}
	else
	{
		if (stat("img/big_cookie.webp", &this->_info) < 0)
		{
			std::cerr << "Error stat file" << std::endl;
		}
		oss << this->_info.st_size;
		std::string size = oss.str();
		this->_content_length = "Content-Length: " + size + "\r\n";
	}
}

void Response::sendHeaders(ParseRequest request)
{
	this->setStatus(request.GetVersion());
	this->setContentType(request.GetPath());
	this->setContentLength(request.GetPath());

	this->_response = this->_status + this->_content_type + this->_content_length + "\r\n";

	if(send(this->_client_fd, this->_response.c_str(), this->_response.size(), 0) == -1)
		std::cerr << "Error while sending." << std::endl;
}

void Response::sendContent(ParseRequest request)
{
	if (request.GetPath() == "/")
	{
		std::ifstream file("index.html");
		std::stringstream buffer;

		buffer << file.rdbuf();
		this->_content = buffer.str();
		if(send(this->_client_fd, this->_content.c_str(), this->_content.size(), 0) == -1)
			std::cerr << "Error while sending." << std::endl;
	}
	else
	{
		std::ifstream ifs("img/big_cookie.webp", std::ios::binary);
		char *buffer = new char[this->_info.st_size];
		ifs.read(buffer, this->_info.st_size);
		int data_sent = 0;
		while(data_sent < this->_info.st_size)
		{
			int data_read = send(this->_client_fd, buffer + data_sent, this->_info.st_size - data_sent, 0);
			if (data_read == -1)
				std::cerr << "Error while sending." << std::endl;
			data_sent += data_read;
		}
		delete[] buffer;
	}
}
