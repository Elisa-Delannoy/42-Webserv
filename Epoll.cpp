#include "Epoll.hpp"

Epoll::Epoll()
{}

Epoll::Epoll(std::vector<int> socket_servers)
{
	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd == - 1)
		return; /*voir pour erreur*/
	for(size_t i = 0; i < socket_servers.size(); i++)
	{
		epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket_servers[i];
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, socket_servers[i], &event) == -1)
		{
			std::cout << "Error socket" << std::endl;
			close(socket_servers[i]);
		}
		else
			this->_servers_event.push_back(event);
	}
}

Epoll::~Epoll()
{
	close(this->_epoll_fd);
}

int Epoll::epollWait()
{
	return epoll_wait(this->_epoll_fd, this->_events, 10, -1);
}

epoll_event Epoll::getEvent(int index)
{
	return this->_events[index];
}

int Epoll::getEpollFd()
{
	return this->_epoll_fd;
}

void Epoll::SetEpoll(int fd)
{
	epoll_event	event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET;
	event.data.fd = fd;
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) 
	{
    	close(fd);
		return; //to do gestion erreur;
	}
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		close (fd);
}


void Epoll::deleteClient(int client_fd)
{
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
		std::cerr << "Delete error" << std::endl;
}
