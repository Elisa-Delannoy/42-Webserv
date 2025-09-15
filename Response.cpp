#include "Response.hpp"

Response::Response()
{ }

Response::Response(int client_fd) : _client_fd(client_fd)
{ }

Response::~Response()
{ }

void Response::displayBody()
{
	std::ifstream file("index.html");
	std::stringstream buffer;
	std::stringstream size;

	buffer << file.rdbuf();
	std::string content = buffer.str();

	size << content.size();
	std::string content_size = size.str();

	//prepare response
	std::string response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: " + content_size + "\r\n"
	"Connection: close\r\n"
	"\r\n" + content;

	//send response
	if(send(this->_client_fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Error while sending." << std::endl;
}

void Response::displayImg()
{
	struct stat img_info;

	if (stat("img/big_cookie.webp", &img_info) < 0)
	{
		std::cerr << "Error stat file" << std::endl;
		return;
	}
	std::ostringstream oss;
	oss << img_info.st_size;
	std::string size = oss.str();
	std::string response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: image/webp\r\n"
	"Content-Length: " + size + "\r\n"
	"Connection: close\r\n"
	"\r\n";

	//SEND HEADERS
	if(send(this->_client_fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Error while sending." << std::endl;

	//SEND BINARY DATAS
	std::ifstream ifs("img/big_cookie.webp", std::ios::binary);
	char *buffer = new char[img_info.st_size];
	ifs.read(buffer, img_info.st_size);
	int data_sent = 0;
	while(data_sent < img_info.st_size)
	{
		int data_read = send(this->_client_fd, buffer + data_sent, img_info.st_size - data_sent, 0);
		if (data_read == -1)
			std::cerr << "Error while sending." << std::endl;
		data_sent += data_read;
	}
	delete[] buffer;
}