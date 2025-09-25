#include "ParseBodyRequest.hpp"
#include <map>
#include <vector>

ParseBody::ParseBody()
{
}

ParseBody::~ParseBody()
{
}

int ParseBody::GetContentLen() const
{
	return _len;
}

std::string ParseBody::GetContentType() const
{
	return _type;
}


int	ParseBody::FindBodyLen(ParseRequest& request)
{
	std::map<std::string, std::string> head = request.GetHeader();
	std::map<std::string, std::string>::iterator it = head.find("Content-Type");
	if (it != head.end())
		this->_type = it->second;
	it = head.find("Content-Length");
	if (it != head.end())
	{
		std::istringstream	ss_body_len(it->second);
		if (!(ss_body_len >> this->_len) || !ss_body_len.eof() || this->_len <= 0)
			return (-1);
		return (this->_len);
		// this->_size_body_buf = atoi(it->second.c_str());
	}
	return (0);
}

void  ParseBody::ChooseContent(char* body)
{
	std::string 		string_body = body;
	std::istringstream	ss_body(string_body);

	// std::cout << "body\n" << string_body << '\n' << std::endl;

	// std::cout << "type" << this->_type << std::endl;
	if (this->_type.find("application/x-www-form-urlencoded") != std::string::npos)
		AppForm(ss_body);
	else if (this->_type.find("application/json") != std::string::npos)
		AppJson(ss_body);
}

void	ParseBody::AppForm(std::istringstream& body) /*voir si return ou si stock map en prive*/
{
	int	tot_len = 0;
	std::string	s_body;
	std::map<std::string, std::string>	body_form;

	while(getline(body, s_body, '&') && tot_len < this->_len)
	{
		if (tot_len + static_cast<int>(s_body.size()) > this->_len) /*FAIRE TEST POUR VERIFIER QUAND BODYlength != reelle length */
		{
			int	i = this->_len - tot_len;
			s_body = s_body.substr(0, i);
		}
		tot_len += s_body.size() + 1;
		size_t	delimiter = s_body.find('=');
		std::string	name;
		std::string	content;
		
		if (delimiter != std::string::npos)
		{
			name = s_body.substr(0, static_cast<int>(delimiter));
			content = s_body.substr(static_cast<int>(delimiter) + 1, static_cast<int>(s_body.size()));
		}
		else
		{
			name = s_body;
			content = "";
		}
		// std::cout << name << " | " << content << std::endl;
		body_form[name] = content;
	}
}

void	ParseBody::AppJson(std::istringstream& body) /*voir si return ou si stock map en prive*/
{
	std::vector<char>	r_body(this->_len);
	body.read(&r_body[0], this->_len);
	if (body.gcount() < this->_len)
		return; /*voir pour erreur*/
	std::string	s_body(r_body.begin(), r_body.end());

	std::map<std::string, std::string>	body_json;

	for (size_t i = 0; i < s_body.size(); i++)
	{
		if (s_body.at(i) == '"' && i < s_body.size() + 2)
		{
			i++;
			std::string	stemp = s_body.substr(i, s_body.size() - i);
			size_t	found = stemp.find("\":\"");
			std::string	name = "";
			if (found != std::string::npos)
				name = stemp.substr(0, found);
			i = found + i;
			if (i < s_body.size() + 3)
				i = i + 3;
			stemp = s_body.substr(i, s_body.size() - i);
			found = stemp.find("\",\"");
			std::string	content = "";
			if (found != std::string::npos)
			{
				content = stemp.substr(0, found);
				i = found + i;
			}
			else
			{
				found = stemp.find_last_of("\"");
				i  = s_body.size();
					if (found != std::string::npos)
						content = stemp.substr(0, found);
			}
			std::cout << "1=" << name << " 2=" << content << std::endl;
			body_json[name] = content;
		}
	}
}

/*DEUXIEME VERSION PLUTOT C*/

// void	ParseBody::AppJson(std::istringstream& body) /*voir si return ou si stock map en prive*/
// {
// 	// std::string	s_body;
// 	// std::map<std::string, std::string>	body_form;

// 	std::vector<char>	r_body(this->_len);
// 	body.read(&r_body[0], this->_len);
// 	if (body.gcount() < this->_len)
// 		return; /*voir pour erreur*/
// 	std::string	s_body(r_body.begin(), r_body.end());
// 	std::cout << "content=" << s_body << "size=" <<  s_body.size()<< std::endl;
// 	std::map<std::string, std::string>	body_json;

// 	for (size_t i = 0; i < s_body.size(); i++)
// 	{
// 		// std::cout << "i=" << i << std::endl;
		
// 		if (s_body.at(i) == '"' && i < s_body.size() + 2)
// 		{
// 			i++;
// 			size_t temp = i;
// 			while (i < s_body.size() + 2)
// 			{
// 				if (s_body.at(i) == '"' && s_body.at(i + 1) == ':' && (s_body.at(i + 2) == '"'))
// 					break;
// 				i++;
// 			}
// 			std::string	name = s_body.substr(temp, i - temp);
// 			std::cout << name << std::endl;
// 			if (i < s_body.size() + 3)
// 				i+=3;
// 			temp = i;
// 			std::cout << i << std::endl;
// 			while (i < s_body.size())
// 			{
// 				if ((i < s_body.size() + 2 && s_body.at(i) == '"' && s_body.at(i + 1) == ',' && (s_body.at(i + 2) == '"'))
// 					|| (i < s_body.size() + 1 &&  s_body.at(i) == '"' && s_body.at(i + 1) == '}'))
// 					break;
// 				i++;
// 			}
// 			std::string content =  s_body.substr(temp, i - temp);
// 			std::cout << "1=" << name << " 2=" << content << std::endl;
// 			body_json[name] = content;
// 		}
// 	}
// }






