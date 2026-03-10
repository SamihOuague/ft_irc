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
#include <cstring>

Server::Server(void)
{
	int opt = 1;
	std::cout << "Server: Default constructor called." << std::endl;

	(*this).sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if ((*this).sockfd == -1)
		std::cerr << "error socket" << std::endl;
	setsockopt((*this).sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	(*this).addr.sin_addr.s_addr = INADDR_ANY;
	(*this).addr.sin_family = AF_INET;
	(*this).addr.sin_port = htons(6667);
	return;
}

Server::Server(Server const &instance)
{
	std::cout << "Server: Copy constructor called." << std::endl;
	*this = instance;
	return;
}

Server &Server::operator=(Server const &instance)
{
	std::cout << "Server: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

std::vector<std::string> extract_cmd(std::string req)
{
	std::vector<std::string> argv;
	std::string cursor(req);
	long unsigned int bnpos = -1;

	//std::cout << "REQ => "<< req << std::endl;
	if (req[0] == ':')
	{
		cursor = req.substr(1);
		//std::cout << "ZBEUB" << req << std::endl;
	}
	for (long unsigned int spos = cursor.find(' '); ; spos = cursor.find(' '))
	{
		bnpos = cursor.find('\r');
		argv.push_back(cursor.substr(0, (cursor.find(' ') == std::string::npos || cursor[0] == ':') ? bnpos : spos));
		if (cursor.find(' ') == std::string::npos || cursor[0] == ':')
			break;
		cursor = cursor.substr(spos + 1);
	}
	return argv;
}

void	Server::forwardMsg(Client *client, std::vector<Client *> clients, std::string &msg)
{
	
	std::cout << msg << std::endl;
	for (int i = 0; i < (int)clients.size(); i++)
	{
		if (clients[i] != client)
			send((*clients[i]).getFd(), msg.c_str(), msg.size(), 0);
	}
}

void	Server::execCmd(Client *client, std::vector<std::string> argv)
{
	std::string msg;
	std::string nick;

	if (argv[0] == "NICK")
	{
		if (argv.size() != 2)
			return;

		nick = argv[1];
		if ((*client).getIsNew())
		{
			msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".\r\n";
			(*client).setIsNew(false);
		}
		else
			msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost NICK :" + nick + "\r\n";
		std::cout << msg << std::endl;
		(*client).setNick(nick);
		send((*client).getFd(), msg.c_str(), msg.size(), 0);
	}
	else if (argv[0] == "JOIN")
	{
		if (argv.size() != 2 || argv[1] == ":")
			return ;
		(*this).channels[argv[1]].addClient(client);
		msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost JOIN :" + argv[1] + "\r\n";
		forwardMsg(client, (*this).channels[argv[1]].getClients(), msg);
		nick = "";
		for (int i = 0; i < (int)(*this).channels[argv[1]].getClients().size(); i++)
			nick += " " + (*(*this).channels[argv[1]].getClients()[i]).getNick();
		msg += ":server 353 " + (*client).getNick() + " = " + argv[1] + " :" + nick + "\r\n";
		msg += ":server 366 " + (*client).getNick() + " " + argv[1] + " :End of /NAMES list\r\n";
		send((*client).getFd(), msg.c_str(), msg.size(), 0);
	}
	else if (argv[0] == "PRIVMSG")
	{
		if (argv.size() != 3)
			return ;
		msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost PRIVMSG " + argv[1] + " " + argv[2] + "\r\n";
		std::cout << msg << std::endl;
		if (argv[1][0] == '#')
			(*this).forwardMsg(client, (*this).channels[argv[1]].getClients(), msg);
		else
		{
			std::map<int, Client>::iterator it = (*this).clients.begin();
			for (int i = 0; i < (int)(*this).clients.size(); i++)
			{
				if ((*it).second.getNick() == argv[1])
				{
					send((*it).second.getFd(), msg.c_str(), msg.size(), 0);
					break ;
				}
				it++;
			}
		}
	}
	else
	{
		//for (int i = 0; i < (int)argv.size(); i++)
		//	std::cout << argv[i] << " ";
		//std::cout << std::endl;
	}
}

void Server::execReq(Client *client)
{
	std::string cursor("");
	std::string cmd("");
	long unsigned int bnpos = -1;
	std::vector<std::string> argv;

	cursor = (*this).readCmd((*client).getFd());
	while (cursor.find('\n') != std::string::npos)
	{
		bnpos = cursor.find('\n');
		cmd = cursor.substr(0, bnpos);
		cursor = cursor.substr(bnpos + 1);
		argv = extract_cmd(cmd);
		(*this).execCmd(client, argv);
	}
}

std::string	Server::readCmd(int const conn_sock)
{
	char buffer[512];
	std::string msg("");

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
	return msg;
}


Server::~Server(void)
{
	std::cout << "Server: Destructor called." << std::endl;
	return;
}

void Server::start(int const port)
{
	Client	client;

	(*this).addr.sin_port = htons(port);

	if (bind((*this).sockfd, (struct sockaddr *)&(*this).addr, sizeof((*this).addr)) == -1)
	{
		std::cerr << "Error binding" << std::endl;
		return;
	}
	std::cout << "Bind success" << std::endl;

	if (listen((*this).sockfd, 10) == -1)
	{
		std::cerr << "Error listen" << std::endl;
		return;
	}
	std::cout << "Server listening on port " << port << std::endl;
	struct epoll_event ev, events[10];
	int listen_sock, conn_sock, nfds, epollfd;

	listen_sock = (*this).sockfd;
	epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
		std::cerr << "Error creating epoll" << std::endl;
		return;
	}
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		std::cerr << "Error epoll_ctl" << std::endl;
		return;
	}
	for (;;)
	{
		nfds = epoll_wait(epollfd, events, 10, -1);
		if (nfds == -1)
		{
			std::cerr << "Error epoll_wait" << std::endl;
			return;
		}
		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == listen_sock)
			{
				conn_sock = client.acceptConnection(listen_sock);
				(*this).clients[conn_sock] = client;
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					std::cerr << "Error epoll_ctl" << std::endl;
					return;
				}
				std::string msg = "CAP * LS :JOIN NICK\r\n";
				std::cout << "Client connected" << std::endl;
				send(conn_sock, msg.c_str(), msg.size(), 0);
				
			}
			else
			{
				(*this).execReq(&(*this).clients[events[n].data.fd]);
			}
		}
	}
}