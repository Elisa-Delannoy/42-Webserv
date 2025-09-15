#include "ParseBodyRequest.hpp"

ParseBody::ParseBody(std::string& type, int len) : _type(type), _len(len)
{
}

ParseBody::~ParseBody()
{
}

void  ParseBody::ChooseContent(std::istringstream& body)
{
	/*if else if pour choisir le parsing*/

	std::cout << "len=" << this->_len << std::endl;
	std::cout << "type=" << this->_type << std::endl;
	std::string	body_line;

	if (this->_type != " application/x-www-form-urlencoded") /*voir pour supprimer les epsaces avant et apres et entre les mots*/
		return;
	std::map<std::string, std::string>	body_form;
	while (getline(body, body_line)) /*ne pas faire getline mais voir pour detecter chaque & i/o \n et delimeter = pour la map*/
	{
		std::string	name;
		std::string	content;
		std::cout << "line=" << body_line << std::endl;
	}
}
	



