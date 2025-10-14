#ifndef _SERVERCONF_
	#define _SERVERCONF_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <climits>
#include <arpa/inet.h>
#include "Location.hpp"

class ServerConf
{
	private:
		std::vector<std::string> _server_name;
		long long _client_body_size;
		std::map<int, std::string> _error_pages;
		std::vector<std::pair<std::string, int> > _host_port;
		std::vector<Location> _location;
	public:
		int _nb_location;
		ServerConf();
		~ServerConf();
		
		void SetServerName(std::string server_name);
		void SetHostPort(std::string host, int port);
		void SetClientBodySize(long long size);
		void SetErrorPage(int type_error, std::string path);
		void SetLocation(Location location);

		std::vector<std::string> GetServerName() const;
		long long GetClientBodySize() const;
		int GetPort(int nb) const;
		std::string GetHost(int nb) const;
		std::string GetErrorPath(int type_error);
		std::map<int, std::string> GetErrorPath();
		Location& GetLocation(int nb);
		std::vector<Location>& GetLocation();
		size_t GetHostPortSize() const;

		int AddServerName(std::string line);
		int AddHostPort(std::string line);
		int AddClientBody(std::string line);
		int AddErrorPage(std::string line);
		int AddLocation(std::ifstream &conf, std::string line);

		bool checkMethods(std::string method, int nb);
		int checkLocation(std::string name);
		std::string removeInlineComment(std::string &line);
		bool isComment(const std::string &line);
		bool HasLocationForExtension(const std::string& name, const std::string ext, Location& okloc);

		void Error(int error);
};

#endif