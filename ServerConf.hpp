#ifndef _SERVERCONF_
	#define _SERVERCONF_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>

class ServerConf
{
	private:
		std::vector<std::string> _server_name;
		int _client_body_size;
		std::map<int, std::string> _error_pages;
		std::vector<std::pair<std::string, int> > _host_port;
	public:
		ServerConf();
		~ServerConf();
		
		void SetServerName(std::string server_name);
		void SetHostPort(std::string host, int port);
		void SetClientBodySize(int size);
		void SetErrorPage(int type_error, std::string path);

		std::vector<std::string> GetServerName() const;
		int GetClientBodySize() const;
		int GetPort(int nb) const;
		std::string GetHost(int nb) const;
		std::string GetErrorPath(int type_error);
};

#endif