#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/epoll.h>
#include <iostream>
#include <vector>

class Epoll
{
	public:
		Epoll();
		Epoll(std::vector<int> socket_servers);
		~Epoll();

		int epollWait();
		void setClientEpollin(int socket_client);
		void SetClientEpollout(int index, int socket_client);
		void deleteClient(int client_fd);

		epoll_event getEvent(int index);
		int getEpollFd();

	private:
		epoll_event _events[10];
		epoll_event _client_event;
		std::vector<epoll_event> _servers_event;
		int _epoll_fd;
};

#endif