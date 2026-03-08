/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:54:36 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/14 09:22:43 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

Server::Server(void)
{
	int	opt = 1;
	std::cout << "Server: Default constructor called." << std::endl;

	(*this).sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if ((*this).sockfd == -1)
		std::cerr << "error socket" << std::endl;
	setsockopt((*this).sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	(*this).addr.sin_addr.s_addr = INADDR_ANY;
	(*this).addr.sin_family = AF_INET;
	(*this).addr.sin_port = htons(6667);
	return ;
}

Server::Server(Server const &instance)
{
	std::cout << "Server: Copy constructor called." << std::endl;
	*this = instance;
	return ;
}

Server&	Server::operator=(Server const &instance)
{	
	std::cout << "Server: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

Server::~Server(void)
{	
	std::cout << "Server: Destructor called." << std::endl;
	return ;
}

void	Server::start(int const port)
{
	(*this).addr.sin_port        = htons(port);

	if (bind((*this).sockfd, (struct sockaddr *)&(*this).addr, sizeof((*this).addr)) == -1)
	{
		std::cerr << "Error binding" << std::endl;
		return ;
	}
	std::cout << "Bind success" << std::endl;

	if (listen((*this).sockfd, 10) == -1)
	{
		std::cerr << "Error listen" << std::endl;
		return ;
	}
	std::cout << "Server listening on port " << port << std::endl;
	struct	epoll_event ev, events[10];
	int listen_sock, conn_sock, nfds, epollfd, socklen;
	struct	sockaddr_in client_addr;

	socklen = sizeof(client_addr);
	listen_sock = (*this).sockfd;
	epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
		std::cerr << "Error creating epoll" << std::endl;
		return ;
	}
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		std::cerr << "Error epoll_ctl" << std::endl;
		return ;
	}
	for (;;)
	{
		nfds = epoll_wait(epollfd, events, 10, -1);
		if (nfds == -1)
		{
			std::cerr << "Error epoll_wait" << std::endl;
			return ;
		}
		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == listen_sock)
			{
				conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, (socklen_t *)&socklen);
				if (conn_sock == -1)
				{
					std::cerr << "Error accept" << std::endl;
					return ;
				}
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					std::cerr << "Error epoll_ctl" << std::endl;
					return ;
				}
			}
			else
				(*this).readData(events[n].data.fd);
		}
	}
}

void	Server::readData(int const conn_sock)
{
	for (char buf = '\0'; read(conn_sock, &buf, 1) > 0; )
	{
		std::cout << buf;
		if (buf == '\n')
			break ;
	}
}