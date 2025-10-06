#include "ParseBodyRequest.hpp"

/*A SUPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPRIMER A LA FIN */
void	printmap(std::map<std::string, std::string>& map)
{
	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
		std::cout << "key: " << it->first << " | value: " << it->second << std::endl;
}

void	printvec1(std::vector<char>::iterator begin, std::vector<char> vec)
{
	for (std::vector<char>::const_iterator test = begin; test != vec.end(); test++)
	{
		std::cout << *(test);
	}
}

void printvecpart(std::vector<ParseBody::Part>& vec)
{
	std::cout << "vector" << std::endl;
	for (std::vector<ParseBody::Part>::const_iterator it = vec.begin(); it != vec.end(); it++)
	{
		std::cout << *(it) << std::endl;
	}
	std::cout << "\n\nfin vector" << std::endl;
}

/*A SUPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPRIMER A LA FIN */

ParseBody::ParseBody() : _len(0), _content_chunk(false)
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

int ParseBody::GetContentLen() const
{
	return this->_len;
}

std::string ParseBody::GetContentType() const
{
	return this->_type;
}

bool ParseBody::GetChunk() const
{
	return this->_content_chunk;
}

void	ParseBody::SetChunk(bool status)
{
	this->_content_chunk = status;
}

// taille en hexa \r\n 
// morceau \r\n 
// fin : taille 0\r\n


// int	HexaToDec(std::string)
// {
// 	std::string	hexa
// }

void	ParseBody::CheckBodyType(std::map<std::string, std::string>& head)
{
	std::map<std::string, std::string>::iterator it;

	it = head.find("Content-Length");
	if (it != head.end())
		FindBodyLen(it);
	else
	{
		it = head.begin();
		it = head.find("Transfer-Encoding");
		if (it != head.end())
			this->_content_chunk = true;
	}
}

void	ParseBody::FindBodyLen(std::map<std::string, std::string>::iterator& it)
{
	std::istringstream	ss_body_len(it->second);
	if (!(ss_body_len >> this->_len) || !ss_body_len.eof() || this->_len <= 0)
		return; /*voir pour send erreur*/
	std::cout << "len = " << this->_len << std::endl;
}

bool	ParseBody::IsBody(ParseRequest& request)
{
	std::map<std::string, std::string> head = request.GetHeader();
	std::map<std::string, std::string>::iterator it = head.find("Content-Type");
	if (it == head.end())
		return (false);
	this->_type = it->second;
	CheckBodyType(head);
	return (true);	
}

void  ParseBody::ChooseContent(std::vector<char> to_parse)
{
	// std::vector<char>	to_parse(body, body + this->_len);

	// if (this->_type.find("application/x-www-form-urlencoded") != std::string::npos)
	// 	AppForm(body);
	if (this->_type.find("application/json") != std::string::npos)
	{
		std::cout << "Appjson" << std::endl;
		AppJson(to_parse);
	}
	else if (this->_type.find("multipart/form-data") != std::string::npos)
	{
		std::cout << "AppMultipart" << std::endl;
		AppMultipart(to_parse);
	}



// text/plain : Envoyer du texte brut sans mise en forme.

// text/html : Envoyer du contenu HTML (rare en requête, plus en réponse)
// application/xml: Envoyer des données au format XML
// application/octet-stream: Envoyer des données binaires (fichiers, blobs).
// application/pdf, image/png, audio/mpeg, etc.: Envoi de types de fichiers spécifiques.
// application/ld+json: JSON-LD (Linked Data), souvent en API sémantique.
// application/graphql: Envoyer des requêtes GraphQL.


// erreur ;
// erreur 400 Bad Request (si la taille ne correspond pas ou est mal formée) 
// 500 Internal Server Error (si le serveur rencontre une erreur de traitement).
	
}

void	ParseBody::AppForm(char* body_req) /*voir si return ou si stock map en prive*/
{
	int	tot_len = 0;
	std::string	s_body = body_req;
	std::istringstream	body(s_body);
	std::map<std::string, std::string>	body_form;
	s_body.clear();

	while(getline(body, s_body, '&') && tot_len < this->_len)
	{
		if (tot_len + static_cast<int>(s_body.size()) > this->_len) /*FAIRE TEST POUR VERIFIER QUAND BODYlength != reelle length */
			s_body = s_body.substr(0, this->_len - tot_len);
		tot_len += s_body.size() + 1;
		size_t		delimiter = s_body.find('=');
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
		body_form[name] = content;
		//HERE RETURN MAP
	}
}

std::string	JsonName(std::string& s_body, size_t& i)
{
	std::string	name;
	std::string	stemp = s_body.substr(i, s_body.size() - i);
	size_t		found = stemp.find("\":\"");

	if (found != std::string::npos)
		name = stemp.substr(0, found);
	else
		name = "";
	i = found + i;
	if (i < s_body.size() + 3)
		i = i + 3;
	return (name);
}

std::string	JsonContent(std::string& s_body, size_t& i)
{
	std::string	content;
	std::string	stemp = s_body.substr(i, s_body.size() - i);
	size_t		found = stemp.find("\",\"");

	if (found != std::string::npos)
	{
		content = stemp.substr(0, found);
		i = found + i;
	}
	else
	{
		found = stemp.find_last_of("\"");
		i = s_body.size();
			if (found != std::string::npos)
				content = stemp.substr(0, found);
	}
	return (content);
}

void	ParseBody::AppJson(std::vector<char> body) /*voir si return ou si stock map en prive*/
{
	if (static_cast<int>(body.size()) < this->_len)
		return; /*voir pour erreur*/
	std::string	s_body(body.begin(), body.end());
	std::map<std::string, std::string>	body_json;

	for (size_t i = 0; i < s_body.size(); i++)
	{
		if (s_body.at(i) == '"' && i < s_body.size() + 2)
		{
			i++;
			std::string	name = JsonName(s_body, i);
			std::string	content = JsonContent(s_body, i);
			body_json[name] = content;
		}
	}
	printmap(body_json);
	//RETURN MAP
}

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
	parts.name =  SelectValue("name=", "\"", head);
	parts.filename =  SelectValue("filename=", "\"", head);
}

std::vector<char>::iterator	FindPart(std::string& boundary, std::vector<char>& body, std::vector<char>::iterator it)
{
	std::vector<char>::iterator	new_it = std::search(it, body.end(), boundary.begin(), boundary.end());
	return (new_it);
}

int	CheckIndex(size_t& index, std::string& boundary, std::vector<char>& r_body, std::vector<char>::iterator& it_head)
{
	
	if (index >= r_body.size() - 4 - boundary.size())
		return (0);
	it_head = FindPart(boundary, r_body, it_head);
	if (it_head < r_body.end())
		it_head += boundary.size();
	if (*(it_head) == '\r')
		it_head++;
	if (*(it_head) == '\n')
		it_head++;
	if (it_head >= r_body.end())
		return (0);
	return (1);
}


size_t		BodyMultipart(std::vector<char>::iterator end_head, std::string& boundary, std::vector<char>& r_body, ParseBody::Part& parts)
{
	std::vector<char>::iterator	it_body = end_head + 4;
	if (it_body >= r_body.end())
		return (-1);
	std::vector<char>::iterator	end_body = FindPart(boundary, r_body, it_body);
	if (end_body >= r_body.end())
		return (-1);
	parts.content = std::vector<char>(it_body, end_body);
	size_t index = std::distance(r_body.begin(), end_body);
	return (index);
}

void	ParseBody::AppMultipart(std::vector<char>& r_body)
{
	int	boud = this->_type.find("boundary=");
	std::string	boundary = this->_type.substr(boud + 9);

	std::vector<char>::iterator	it_head = r_body.begin();
	std::vector<Part>	body_request;
	size_t	index = 0;

	while (1)
	{
		if(!CheckIndex(index, boundary, r_body, it_head))
			break;
		std::string	end = "\r\n\r\n";
		std::vector<char>::iterator	end_head = FindPart(end, r_body, it_head);
		if (end_head != r_body.end())
		{
			Part	parts;
			std::string	head(it_head, end_head);
			SetPart(parts, head);
			index = BodyMultipart(end_head, boundary, r_body, parts);
			if (index == 0)
				break;
			this->_multipart.push_back(parts);
		}
	}
	// printvecpart(this->_multipart);
	// std::ofstream out("uploads/fichier.png", std::ios::binary);
	// out.write(parts.content.data(), parts.content.size());
	// out.close();

}
