#include "ParseBody.hpp"

ParseBody::ParseBody() : _len(0), _line(0), _content_chunk(false)
{
}

ParseBody::~ParseBody()
{
}

bool	ParseBody::IsBody(ParseRequest& request)
{
	std::map<std::string, std::string> head = request.GetHeader();
	std::map<std::string, std::string>::iterator it = head.find("Content-Type");

	if (it != head.end())
		this->_type = it->second;
	int	isbody = CheckBodyType(head);
	if (isbody == 0)
		return (false);
	if (isbody == -1)
		request.SetForError(true, 400);
	return (true);
}

int	ParseBody::CheckBodyType(std::map<std::string, std::string>& head)
{
	std::map<std::string, std::string>::iterator it;
	it = head.find("Content-Length");
	if (it != head.end())
	{
		if (FindBodyLen(it) == -1)
			return (-1);
		return (1);
	}
	else
	{
		it = head.begin();
		it = head.find("Transfer-Encoding");
		if (it != head.end())
		{
			this->_content_chunk = true;
			return (1);
		}
	}
	return (0);
}

int	ParseBody::FindBodyLen(std::map<std::string, std::string>::iterator& it)
{
	std::istringstream	ss_body_len(it->second);
	if (!(ss_body_len >> this->_len) || !ss_body_len.eof() || this->_len <= 0)
		return (-1);
	return (0);
	return this->_content_chunk;
}

int	ConvertChunkSize(std::string to_convert)
{
	int i;
	std::istringstream size(to_convert);

	if (!(size >> std::hex >> i))
		return (-1);
	return (i);	
}

int	ParseHexa(std::vector<char>& content, std::vector<char>::iterator& start,
	int& size, std::vector<char>::iterator& it)
{
	const char* endhexa = "\r\n";

	std::vector<char>::iterator end = std::search(start, content.end(), 
		endhexa, endhexa + 2);
	if (end == content.end())
		return (-1);
	std::string	to_convert(start, end);
	size = ConvertChunkSize(to_convert);
	it = end + 1;
	if (size == -1)
		return (-1);
	if (size == 0)
		return (1);
	return (0);
}

int	ParseBody::ParseContent(std::vector<char>& content, std::vector<char>::iterator& start,
	int& size, std::vector<char>::iterator& it)
{
	const char* endcontent= "\r\n";

	std::vector<char>::iterator end = std::search(start, content.end(), 
		endcontent, endcontent + 2);
	if (end == content.end())
		return (-1);
	it = end + 1;
	int	len_body = std::distance(start, end);
	if (len_body != size)
		return (-1);
	this->_body.insert(this->_body.end(), start, end);
	return (1);
}

int	ParseBody::ParseChunk(std::vector<char>& content)
{
	int size = 0;

	for (std::vector<char>::iterator it = content.begin(); it != content.end(); it++)
	{
		std::vector<char>::iterator start = it;
		if (this->_line % 2 == 0)
		{
			int check = ParseHexa(content, start, size, it);
			if (check == -1)
				return (-1);
			if (check == 1)
				return (1);	
			this->_line++;
		}
		else if (this->_line % 2 != 0)
		{
			int check = ParseContent(content, start, size, it);
			if (check == -1)
				return (-1);
			this->_line++;
		}
	}
	this->_line = 0;
	return (1);
}

void	ParseBody::ClearBody()
{
	this->_len = 0;
	this->_line = 0;
	this->_type.clear();
	this->_body.clear();
	this->_content_chunk = false;
}

int ParseBody::GetContentLen() const
{
	return (this->_len);
}

bool ParseBody::GetChunk() const
{
	return (this->_content_chunk);
}

std::string ParseBody::GetBody() const
{
	std::string str(_body.begin(), _body.end());
	return str;
}

std::string ParseBody::GetContentType() const
{
	return (this->_type);
}

void	ParseBody::SetChunk(bool status)
{
	this->_content_chunk = status;
}

void	ParseBody::SetBody(std::vector<char> body)
{
	_body = body;
}
