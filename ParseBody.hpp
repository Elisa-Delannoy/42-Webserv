#ifndef ParseBody_HPP
# define ParseBody_HPP

# include "ParseRequest.hpp"
# include <map>
# include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>

class ParseBody
{
	private:
		int					_len;
		int					_line;
		std::string			_type;
		std::vector<char>	_body;
		bool				_content_chunk;

	public:
		ParseBody();
		~ParseBody();

		struct Part
		{
			std::string	type;
			std::string	name;
			std::string	filename;
			std::vector<char>	content;

			friend std::ostream& operator<<(std::ostream& out, const Part& parts)
			{
				out << "Type: " << parts.type << " Name: " << parts.name << ", Filename: " << parts.filename << ", Content: ";
				if (!parts.content.empty())
				{
					for(std::vector<char>::const_iterator it = parts.content.begin(); it != parts.content.end(); it++)
					{
						// out << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)*(it);
						out << *it;
					}
					out << std::endl;
				}
				return (out);
			}
		};
		
		void	ChooseContent(std::vector<char> body);
		void	AppForm(char* body_req);
		void	AppJson(std::vector<char> body);
		void	AppMultipart(std::vector<char>& body);

		bool	IsBody(ParseRequest& request);
		int		CheckBodyType(std::map<std::string, std::string>& head);
		int		FindBodyLen(std::map<std::string, std::string>::iterator& it);
		int		ParseChunk(std::vector<char>& content);
		int		ParseContent(std::vector<char>& content, std::vector<char>::iterator& start,
					int& size, std::vector<char>::iterator& it);

		int		GetContentLen() const;
		bool	GetChunk() const;
		std::string GetContentType() const;

		void	SetChunk(bool chunk);
		void	SetBody(std::vector<char> body);
		
		std::vector<ParseBody::Part> _multipart;
};


#endif

