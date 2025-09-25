#ifndef PARSEBODYREQUEST_HPP
# define PARSEBODYREQUEST_HPP

# include "ParseRequest.hpp"
# include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class ParseBody
{
	private:
		std::string	_type;
		int			_len;

	public:
		ParseBody();
		~ParseBody();
		void	ChooseContent(char* body);
		void	AppForm(std::istringstream& body);
		void	AppJson(std::istringstream& body);
		int		FindBodyLen(ParseRequest& request);

		int GetContentLen() const;
		std::string GetContentType() const;
};


#endif