#ifndef _SERVERCONF_
	#define _SERVERCONF_

#include <iostream>
#include <string>
#include <map>
#include <vector>

class ServerConf
{
	private:
		std::vector<std::string> _server_name;
		std::string _host;
		int _port;
		int _client_body_size;
		std::map<int, std::string> _error;
	public:
		ServerConf();
		~ServerConf();
		
		void SetServerName(std::string server_name);
		void SetHost(std::string host);
		void SetPort(int port);
		void SetClientBodySize(int size);

		std::vector<std::string> GetServerName() const;
};

#endif