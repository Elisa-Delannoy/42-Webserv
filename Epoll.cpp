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

void Epoll::SetEpoll(int fd, uint32_t flag)
{
	epoll_event	event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		if (errno == EEXIST)
		{
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
				std::cerr << "Modification error: " << strerror(errno) << std::endl;
		}
		else
		{
			std::cerr << "Error : " << strerror(errno) << std::endl;
			close(fd);
		}
	}	
}


void Epoll::deleteClient(int client_fd)
{
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
		std::cerr << "Delete error" << std::endl;
}
