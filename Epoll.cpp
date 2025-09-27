#include "Epoll.hpp"

Epoll::Epoll()
{}

Epoll::Epoll(std::vector<int> socket_servers)
{
	this->_epoll_fd = epoll_create1(0); /*verifier  si -1*/
	for(size_t i = 0; i < socket_servers.size(); i++)
	{
		epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket_servers[i];
		this->_servers_event.push_back(event);
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, socket_servers[i], &this->_servers_event[i]);
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

void Epoll::setClientEpollin(int socket_client)
{
	this->_client_event.events = EPOLLIN;
	this->_client_event.data.fd = socket_client;
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, socket_client, &this->_client_event);
}

void Epoll::SetClientEpollout(int index, int socket_client)
{
	this->_events[index].events = EPOLLOUT;
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, socket_client, &this->_client_event);
}

void Epoll::deleteClient(int client_fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
}
