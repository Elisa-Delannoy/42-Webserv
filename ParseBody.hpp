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

		void	ClearBody();
		bool	IsBody(ParseRequest& request);
		int		CheckBodyType(std::map<std::string, std::string>& head);
		int		FindBodyLen(std::map<std::string, std::string>::iterator& it);
		int		ParseChunk(std::vector<char>& content);
		int		ParseContent(std::vector<char>& content, std::vector<char>::iterator& start,
					int& size, std::vector<char>::iterator& it);

		int		GetContentLen() const;
		bool	GetChunk() const;
		std::string GetBody() const;
		std::string GetContentType() const;

		void	SetChunk(bool chunk);
		void	SetBody(std::vector<char> body);
};
#endif
