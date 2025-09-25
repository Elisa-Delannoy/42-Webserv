#include "ParseBodyRequest.hpp"
#include <map>
#include <vector>

ParseBody::ParseBody()
{
	Part	parts;
	parts.name = "";
	parts.filename = "";
	parts.type = "";
	parts.content.push_back('\0');
}

ParseBody::~ParseBody()
{
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
	std::vector<char>	to_parse(body, body + this->_len);


	// std::string 		string_body = body;
	// std::istringstream	ss_body(string_body);

	// std::cout << "body\n" << string_body << '\n' << std::endl;
	std::cout << "type:" << this->_type << std::endl;

	// if (this->_type.find("application/x-www-form-urlencoded") != std::string::npos)
	// 	AppForm(ss_body);
	// else if (this->_type.find("application/json") != std::string::npos)
	// 	AppJson(ss_body);
	if (this->_type.find("multipart/form-data") != std::string::npos)
		AppMultipart(to_parse);
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


std::string	SelectValue(std::string to_find, std::string sep, std::string body)
{
	size_t start = body.find(to_find);
	if (start == std::string::npos)
		return ("");
	start += to_find.size();
	if (sep == "\"")
		start++;
	size_t	end = body.find(sep, start);
	if (end == std::string::npos)
		return (body.substr(start));
	else
		return (body.substr(start, end - start));
}

void	SetPart(ParseBody::Part& parts, std::string head)
{
	parts.type = SelectValue("Content-Disposition:", ";", head);
	// std::cout << "type|" << parts.type << std::endl;
	parts.name =  SelectValue("name=", "\"", head);
	// std::cout << "name|" <<  parts.name << std::endl;
	parts.filename =  SelectValue("filename=", "\"", head);
	// std::cout << "filename|" << parts.filename << std::endl;
}

// void SetContent(ParseBody::Part& parts, std::istringstream& body)
// {
// 	parts.content = 
// 	if (s_body == "--" + boundary || (boundary.size() >= 1 && s_body.size() >= 1 + boundary.size() && s_body.compare(2, boundary.size() - 1, boundary, 0, boundary.size() - 1)==0))
// 	{
// 		std::cout << "content=" << ctn << std::endl;
// 		if (contt == true)
// 			parts.content = std::vector<char>(ctn.begin(), ctn.end());
// 		v_body.push_back(parts);
// 		contt = false;
// 		break;
// 	}

// }

std::vector<char>::iterator	FindPart(std::string& boundary, std::vector<char>& body, std::vector<char>::iterator it)
{
	std::vector<char>::iterator	new_it = std::search(it, body.end(), boundary.begin(), boundary.end());
	return (new_it);
}
void	ParseBody::AppMultipart(std::vector<char>& r_body)
{
	int	boud = this->_type.find("boundary=");
	std::string	boundary = this->_type.substr(boud + 9);
	std::vector<char>::iterator	it_head = r_body.begin();
	std::vector<Part>	body_request;
	size_t	index = 0;
	Part	parts;

	while (1)
	{
		if (index >= r_body.size() - 4 - boundary.size())
			break ;
		it_head = FindPart(boundary, r_body, it_head);
		if (it_head < r_body.end())
			it_head += boundary.size();
		if (*(it_head) == '\r')
			it_head++;
		if (*(it_head) == '\n')
			it_head++;
		if (it_head >= r_body.end())
			break;

		std::string	end = "\r\n\r\n";
		std::vector<char>::iterator	end_head = FindPart(end, r_body, it_head);
		if (end_head != r_body.end())
		{
			std::string	head(it_head, end_head);
			SetPart(parts, head);
			std::vector<char>::iterator	it_body = end_head + 4;
			if (it_body >= r_body.end())
				break;
			std::vector<char>::iterator	end_body = FindPart(boundary, r_body, it_body);
			if (end_body >= r_body.end())
				break;
			parts.content = std::vector<char>(it_body, end_body);
			body_request.push_back(parts);
			// for (std::vector<char>::const_iterator test = end_body; test != r_body.end(); test++)
			// {
			// 	std::cout << *(test);
			// }
			index = std::distance(r_body.begin(), end_body);
		}
	}
	// std::cout << "vector" << std::endl;
	// for (std::vector<Part>::const_iterator it = body_request.begin(); it != body_request.end(); it++)
	// {
	// 	std::cout << *(it) << std::endl;
	// }
	// std::cout << "\n\nfin vector" << std::endl;


	std::ofstream out("uploads/fichier.png", std::ios::binary);
	out.write(parts.content.data(), parts.content.size());
	out.close();

}
