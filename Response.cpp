#include "Response.hpp"

Response::Response(int client_fd, int body_len) : _client_fd(client_fd), _body_len(body_len)
{ }

Response::~Response()
{ }

std::string Response::setStatus(std::string version)
{
	return (version + " 200 OK\r\n");
}

std::string Response::setContentType(std::string path)
{
	std::string ret = "Content-Type: ";
	std::string type;
	if (path == "/")
	{
		ret += "text/html";
	}
	else if (path == "/favicon.ico")
	{
		ret += "image/jpeg";
	}
	else if (path == "/upload")
	{
		ret += "image/webp";
	}
	else
	{
		ret += "image/jpeg";
	}
	return (ret + "\r\n");
}

std::string Response::setSize(const char* path_image)
{
	std::ostringstream oss;
	if (stat(path_image, &this->_info) < 0)
	{
		std::cerr << "Error stat file" << std::endl;
	}
	oss << this->_info.st_size;
	this->_body_len = this->_info.st_size;
	return oss.str();
}

std::string Response::setContentLength(std::string path)
{
	std::ostringstream oss;
	std::string size;
	if (this->_body_len > 0)
	{
		oss << this->_body_len;
		size = oss.str();
	}
	else if (path == "/")
	{
		size = setSize("html/index.html");
	}
	else if (path == "/favicon.ico")
	{
		size = setSize("img/favicon.jpeg");
	}
	else
	{
		size = setSize("img/cookie.jpeg");
	}
	return "Content-Length: " + size + "\r\n";
}

void Response::sendHeaders(ParseRequest header)
{
	this->_status = setStatus(header.GetVersion());
	this->_content_type = setContentType(header.GetPath());
	this->_content_length = setContentLength(header.GetPath());

	this->_response = this->_status + this->_content_type + this->_content_length + "\r\n";

	if(send(this->_client_fd, this->_response.c_str(), this->_response.size(), 0) == -1)
		std::cerr << "Error while sending headers." << std::endl;
}

void Response::sendImage(std::string path_image)
{
	std::ifstream ifs(path_image.c_str() + 1, std::ios::binary); //c.str() + 1 to skip first '/'
	char *buffer = new char[this->_body_len];
	ifs.read(buffer, this->_body_len);
	int data_sent = 0;
	while(data_sent < this->_body_len)
	{
		ssize_t data_read = send(this->_client_fd, buffer + data_sent, this->_body_len - data_sent, 0);
		if (data_read == -1)
			std::cerr << "Error while sending content." << std::endl;
		data_sent += data_read;
	}
	delete[] buffer;
}

void Response::sendBody(ParseRequest request, char* buf)
{
	std::string path = request.GetPath();
	if (path == "/")
	{
		std::ifstream file("html/index.html");
		std::stringstream buffer;

		buffer << file.rdbuf();
		this->_content = buffer.str();
		if(send(this->_client_fd, this->_content.c_str(), this->_content.size(), 0) == -1)
			std::cerr << "Error while sending content." << std::endl;
	}
	else if (path == "/favicon.ico")
	{
		sendImage(path);
	}
	else if (path == "/upload")
	{
		std::string boundary;
		for(int i = 0; buf[i] != '\r'; i++)
			boundary += buf[i];

		int i = 0;
		for(; i < this->_body_len; i++)
		{
			if (buf[i] == 'R' && buf[i+1] == 'I' && buf[i+2] == 'F' && buf[i+3] == 'F')
				break;
		}

		std::cout << std::endl;
		std::cout << "BODY : " << buf+i << std::endl;

		int j = i;
		for(; j < this->_body_len; j++)
		{
			if (buf[j] == '-' && buf[j + 1] == '-' && buf[j + 2] == '-'
				&& buf[j+3] == '-' && buf[j + 4] == '-' && buf[j + 5] == '-'
				&& buf[j+6] == 'g')
				break;
		}
		int endfile = j;
		if (buf[j-2] == '\r' && buf[j-1] == '\n')
			endfile = j - 2;
	
		std::ofstream out("uploads/fichier.png", std::ios::binary);
		out.write(buf + i, endfile - i);
		out.close();
	}
	else
	{
		sendImage(path);
	}
}
