#ifndef _SERVERCONF_
	#define _SERVERCONF_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstdlib>

#include "Location.hpp"

class ServerConf
{
	private:
		std::vector<std::string> _server_name;
		int _client_body_size;
		std::map<int, std::string> _error_pages;
		std::vector<std::pair<std::string, int> > _host_port;
		std::vector<Location> _location;
	public:
		int _nb_location;
		ServerConf();
		~ServerConf();
		
		void SetServerName(std::string server_name);
		void SetHostPort(std::string host, int port);
		void SetClientBodySize(int size);
		void SetErrorPage(int type_error, std::string path);
		void SetLocation(Location location);

		std::vector<std::string> GetServerName() const;
		int GetClientBodySize() const;
		int GetPort(int nb) const;
		std::string GetHost(int nb) const;
		std::string GetErrorPath(int type_error);
		std::map<int, std::string> GetErrorPath();
		Location& GetLocation(int nb);

		size_t GetHostPortSize() const;

		void AddServerName(std::string& line);
		void AddHostPort(std::string& line);
		void AddClientBody(std::string& line);
		void AddErrorPage(std::string& line);
		void AddLocation(std::ifstream& conf);
};

#endif