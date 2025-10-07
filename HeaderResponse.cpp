#include "HeaderResponse.hpp"

HeaderResponse::HeaderResponse(ServerConf & servers, Clients* client, std::string & path, std::string version) :
Response(servers, client) , _path(path), _version(version)
{
	this->_connection = setConnection(client);
}

HeaderResponse::~HeaderResponse()
{ }

void HeaderResponse::sendHeader()
{
	this->_header = this->_status + this->_content_type
		+ this->_content_length + this->_connection + "\r\n";

	if(send(this->_client_fd, this->_header.c_str(), this->_header.size(), 0) == -1)
		std::cerr << "Error while sending headers." << std::endl;
}

void HeaderResponse::setHeader(int code)
{
	if (code == 200)
	{
		this->_status = setStatus(" 200 OK\r\n");
		if (this->_content_length.empty())
			this->_content_length = setContentLength();
	}
	if (code == 204)
	{
		this->_status = setStatus(" 204 No Content\r\n");
		this->_content_length = "Content-Length: 0\r\n";
		return ;
	}
	if (code == 404)
	{
		this->_status = setStatus(" 404 Not Found\r\n");
		this->_content_length = "Content-Length: 0\r\n";
		return ;
	}
	if (code == 500)
	{
		this->_status = setStatus(" 500 Internal Server Error\r\n");
		this->_content_length = "Content-Length: 0\r\n";
	}
	this->_content_type = setContentType();
}

std::string HeaderResponse::setStatus(std::string code)
{
	return (this->_version + code);
}

std::string HeaderResponse::setContentType()
{
	std::string ret = "Content-Type: ";
	if (this->_path == "/")
	{
		ret += "text/html";
	}
	else
	{
		size_t i = this->_path.size() - 1;
		while (this->_path[i - 1] && this->_path[i - 1] != '.')
			i--;
		std::string type = this->_path.substr(i);

		if (type == "jpg" || type == "jpeg" || type == "png" || type == "gif"
			|| type == "svg" || type == "webp" || type == "ico" || type == "avif")
			ret += "image/" + type;
		if (type == "css")
			ret += "text/css";
		if (type == "html")
			ret += "text/html";
		if (type == "js")
			ret += "application/javascript";
		if (type == "pdf" || type == "zip")
			ret += "application/" + type;
	}
	return (ret + "\r\n");
}

std::string HeaderResponse::setContentLength()
{
	std::ostringstream oss;
	std::string size;
	if (this->_body_len > 0)
	{
		oss << this->_body_len;
		size = oss.str();
	}
	else
	{
		size = setSize(this->_path.c_str());
	}
	return "Content-Length: " + size + "\r\n";
}

//get file size with stat function and return it in a string
std::string HeaderResponse::setSize(const char* path_image)
{
	struct stat info;
	std::ostringstream oss;
	if (stat(path_image, &info) < 0)
	{
		std::cerr << "Error stat file" << std::endl;
	}
	oss << info.st_size;
	this->_body_len = info.st_size;
	return oss.str();
}

std::string HeaderResponse::setConnection(Clients* client)
{
	std::string ret = "Connection:";
	std::string status;

	status = getValueHeader(client, "Connection");
	if (status.empty())
		status = " keep-alive\r\n";
	ret += status;
	std::cout << "Connection : " << ret << std::endl;
	ret += "\r\n";

	if (status == " keep-alive")
		this->_close_alive = 1;
	else
		this->_close_alive = 0;

	return ret;
}

int HeaderResponse::getCloseAlive()
{
	return this->_close_alive;
}

std::string HeaderResponse::getValueHeader(Clients* client, std::string key)
{
	std::map<std::string, std::string> map_header = client->_head.GetHeader();
	std::map<std::string, std::string>::iterator it;
	std::string ret;

	it = map_header.find(key);
	if (it != map_header.end())
		ret = it->second;
	return ret;
}

void HeaderResponse::setPath(std::string path)
{
	this->_path = path;
}
