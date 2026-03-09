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
	std::cout << "Client: Default constructor called." << std::endl;
	return;
}

Client::Client(Client const &instance)
{
	std::cout << "Client: Copy constructor called." << std::endl;
	*this = instance;
	return;
}

int Client::getFd(void) const
{
	return (*this).fd;
}

Client &Client::operator=(Client const &instance)
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

std::vector<std::string> extract_cmd(std::string req)
{
	std::vector<std::string> argv;
	std::string cursor(req);
	long unsigned int bnpos = -1;

	for (long unsigned int spos = cursor.find(' '); spos != std::string::npos; spos = cursor.find(' '))
	{
		bnpos = cursor.find(' ');
		if (bnpos == std::string::npos)
			break;
		else
			argv.push_back(cursor.substr(0, bnpos));
		cursor = cursor.substr(bnpos + 1);
	}
	bnpos = cursor.find('\r');
	argv.push_back(cursor.substr(0, bnpos));
	return argv;
}

void exec_cmd(int conn_sock, std::vector<std::string> argv)
{
	std::string welcome;
	std::string nick;

	if (argv[0] == "NICK")
	{
		if (argv.size() != 2)
			return ;
			
		nick = argv[1];
		welcome = ":localhost 001 " + nick +" :Welcome to the server " + nick + ".\r\n";
		send(conn_sock, welcome.c_str(), welcome.size(), 0);
	}
}

void exec_req(int conn_sock, std::string msg)
{
	std::string cursor(msg);
	std::string cmd("");
	long unsigned int bnpos = -1;
	std::vector<std::string> argv;

	while (cursor.find('\n') != std::string::npos)
	{
		bnpos = cursor.find('\n');
		cmd = cursor.substr(0, bnpos);
		cursor = cursor.substr(bnpos + 1);
		std::cout << std::endl
				  << "########" << std::endl;
		std::cout << cmd << std::endl;
		argv = extract_cmd(cmd);
		exec_cmd(conn_sock, argv);
	}
}

void Client::readCmd(void)
{
	char buffer[512];
	std::string msg("");
	int conn_sock = (*this).fd;

	while (true)
	{
		int bytes = recv(conn_sock, buffer, 511, 0);

		if (bytes <= 0)
			break;
		buffer[bytes] = '\0';
		msg = msg + std::string(buffer);
		if (buffer[bytes - 1] == '\n' && buffer[bytes - 2] == '\r')
			break;
	}
	exec_req(conn_sock, msg);
	return;
}

Client::~Client(void)
{
	std::cout << "Client: Destructor called." << std::endl;
	return;
}
