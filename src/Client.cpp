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

Client::Client(void)
{
	std::cout << "Client: Default constructor called." << std::endl;
	return ;
}

Client::Client(Client const &instance)
{
	std::cout << "Client: Copy constructor called." << std::endl;
	*this = instance;
	return ;
}

int	Client::getFd(void) const
{
	return (*this).fd;
}

Client&	Client::operator=(Client const &instance)
{	
	std::cout << "Client: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	(*this).fd = instance.fd;
	(*this).addr = instance.addr;
	(*this).socklen = instance.socklen;
	(*this).username = instance.username;
	return (*this);
}


int		Client::acceptConnection(int const &sockfd)
{
	(*this).fd = accept(sockfd, (struct sockaddr *)&(*this).addr, (socklen_t *)&(*this).socklen);
	if ((*this).fd == -1)
	{
		std::cerr << "Error accept" << std::endl;
		return -1;
	}
	return (*this).fd;
}

void	welcomeMsg(int conn_sock, std::string msg)
{
	std::string	cursor(msg);
	std::string cmd("");
	std::string welcome("");
	std::string nick("");
	long unsigned int	bnpos = -1;

	while (cursor.find('\n') != std::string::npos)
	{
		bnpos = cursor.find('\n');
		cmd = cursor.substr(0, bnpos);
		cursor = cursor.substr(bnpos + 1);
		bnpos = cmd.find(' ');
		if (bnpos == std::string::npos)
			continue;
		else if (cmd.substr(0, bnpos) == std::string("NICK"))
		{
			nick = cmd.substr(bnpos + 1, cmd.size() - (bnpos + 2));
			welcome = ":localhost 001 " + nick + " :Welcome to the server " + nick +".\r\n";
			send(conn_sock, welcome.c_str(), welcome.size(), 0);
		}
	}
}

void	Client::readCmd(void)
{
	char buffer[512];
	std::string	msg("");
	int	conn_sock = (*this).fd;

	while (true)
	{
		int bytes = recv(conn_sock, buffer, 511, 0);

		if (bytes <= 0)
			break;
		buffer[bytes] = '\0';
		msg = msg + std::string(buffer);
		if (buffer[bytes - 1] == '\n' && buffer[bytes - 2] == '\r')
			break ;
	}
	std::cout << msg << std::endl;
	welcomeMsg(conn_sock, msg);
	return ;
}

Client::~Client(void)
{	
	std::cout << "Client: Destructor called." << std::endl;
	return ;
}
