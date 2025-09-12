#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/epoll.h>
#include <iostream>

class Epoll
{
	public:
		Epoll();
		Epoll(int socket_server);
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
		epoll_event _server_event;
		int _epoll_fd;
};

#endif