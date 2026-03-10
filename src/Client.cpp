/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:54:36 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/14 09:22:43 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <vector>
#include <unistd.h>

Client::Client(void): fd(-1), isNew(true), isAuth(false)
{
	//std::cout << "Client: Default constructor called." << std::endl;
	return;
}

Client::Client(Client const &instance)
{
	//std::cout << "Client: Copy constructor called." << std::endl;
	*this = instance;
	return;
}

int Client::getFd(void) const
{
	return (*this).fd;
}

bool	Client::getIsNew(void) const
{
	return (*this).isNew;
}

void	Client::setIsNew(bool value)
{
	(*this).isNew = value;
	return ;
}

Client &Client::operator=(Client const &instance)
{
	//std::cout << "Client: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	(*this).fd = instance.fd;
	(*this).addr = instance.addr;
	(*this).nick = instance.nick;
	(*this).isNew = instance.isNew;
	return (*this);
}

int Client::acceptConnection(int const &sockfd)
{
	unsigned int	socklen = sizeof((*this).addr);

	(*this).fd = accept(sockfd, (struct sockaddr *)&(*this).addr, &socklen);
	if ((*this).fd == -1)
	{
		std::cerr << "Error accept" << std::endl;
		return -1;
	}
	return (*this).fd;
}

bool	Client::getIsAuth(void) const
{
	return ((*this).isAuth);
}

void	Client::setIsAuth(bool isAuth)
{
	(*this).isAuth = isAuth;
	return;
}

void	Client::setNick(std::string const &nick)
{
	(*this).nick = nick;
	return;
}

std::string	Client::getNick(void) const
{
	return (*this).nick;
}

void	Client::sendMsg(std::string const msg)
{
	if (msg.size() <= 0)
		return ;
	send((*this).getFd(), msg.c_str(), msg.size(), 0);
	return ;
}

void	Client::disconnect(int & epollfd)
{
	if (epoll_ctl(epollfd, EPOLL_CTL_DEL, (*this).getFd(), NULL) == -1)
		std::cerr << "Error epoll_ctl delete" << std::endl;
	std::cout << "Client " << (*this).nick << " disconnected" << std::endl;
	close((*this).getFd());
	return ;
}

Client::~Client(void)
{
	//std::cout << "Client: Destructor called." << std::endl;
	return;
}
