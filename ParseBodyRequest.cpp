#include "ParseBodyRequest.hpp"
#include <map>

ParseBody::ParseBody(std::string& type, int len) : _type(type), _len(len)
{
}

ParseBody::~ParseBody()
{
}

void  ParseBody::ChooseContent(std::istringstream& body)
{
	if (this->_type.find("application/x-www-form-urlencoded"))
		Form(body);
}

void	ParseBody::Form(std::istringstream& body) /*voir si return ou si stock map en prive*/
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
		std::cout << name << " | " << content << std::endl;
		body_form[name] = content;
	}

	// std::vector<char>	r_body(this->_len);
	// body.read(&r_body[0], this->_len);
	// std::string	s_body(r_body.begin(), r_body.end());
	// std::cout << "content=" << s_body << std::endl;
}

	



