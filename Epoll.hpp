#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <cerrno>
#include <fcntl.h>

class Epoll
{
	public:
		Epoll();
		Epoll(std::vector<int> socket_servers);
		~Epoll();

		int		epollWait();
		int		SetEpoll(int fd);
		void	deleteClient(int client_fd);

		int	getEpollFd();
		epoll_event	getEvent(int index);

	private:
		epoll_event _events[10];
		std::vector<epoll_event> _servers_event;
		int _epoll_fd;
};


#endif