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

Client::Client(void)
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

Client &Client::operator=(Client const &instance)
{
	//std::cout << "Client: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	(*this).fd = instance.fd;
	(*this).addr = instance.addr;
	(*this).socklen = instance.socklen;
	(*this).nick = instance.nick;
	return (*this);
}

int Client::acceptConnection(int const &sockfd)
{
	(*this).fd = accept(sockfd, (struct sockaddr *)&(*this).addr, (socklen_t *)&(*this).socklen);
	if ((*this).fd == -1)
	{
		std::cerr << "Error accept" << std::endl;
		return -1;
	}
	return (*this).fd;
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

Client::~Client(void)
{
	//std::cout << "Client: Destructor called." << std::endl;
	return;
}
