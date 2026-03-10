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

Server::Server()
{
	std::cout << "Server: Default constructor called." << std::endl;
	return;
}

Server::Server(unsigned short port, std::string password) : password(password)
{
	int opt = 1;
	std::cout << "Server: Default constructor called." << std::endl;

	(*this).sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if ((*this).sockfd == -1)
	{
		std::cerr << "error socket" << std::endl;
		return;
	}
	setsockopt((*this).sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	(*this).addr.sin_addr.s_addr = INADDR_ANY;
	(*this).addr.sin_family = AF_INET;
	(*this).addr.sin_port = htons(port);
	return;
}

Server::Server(Server const &instance)
{
	std::cout << "Server: Copy constructor called." << std::endl;
	*this = instance;
	return;
}

Client	*Server::getClient(std::string &nick)
{
	std::map<int, Client>::iterator	it = (*this).clients.begin();

	for (int i = 0; i < (int) (*this).clients.size(); i++)
	{
		if ((*it).second.getNick() == nick)
			return (&(*it).second);
		it++;
	}
	return (NULL);
}

void	Server::removeClient(Client *client)
{
	std::map<int, Client>::iterator it = (*this).clients.begin();
	std::map<std::string, Channel>::iterator itc = (*this).channels.begin();

	for (int i = 0; i < (int)(*this).channels.size(); i++)
	{
		(*itc).second.removeClient(client, "QUIT :Client disconnected"); 
		itc++;
	}
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		if (&(*it).second == client)
		{
			(*this).clients.erase(it);
			break ;
		}
		it++;
	}
}

Server &Server::operator=(Server const &instance)
{
	std::cout << "Server: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	(*this).addr = instance.addr;
	(*this).sockfd = instance.sockfd;
	(*this).channels = instance.channels;
	(*this).clients = instance.clients;
	(*this).password = instance.password;
	return (*this);
}

Server::~Server(void)
{
	std::cout << "Server: Destructor called." << std::endl;
	return;
}

int Server::init()
{
	if (bind((*this).sockfd, (struct sockaddr *)&(*this).addr, sizeof((*this).addr)) == -1)
	{
		std::cerr << "Error binding" << std::endl;
		return -1;
	}
	std::cout << "Bind success" << std::endl;
	if (listen((*this).sockfd, 10) == -1)
	{
		std::cerr << "Error listen" << std::endl;
		return -1;
	}
	std::cout << "Server listening on port " << ntohs((*this).addr.sin_port) << std::endl;
	return (0);
}

int Server::initPoll()
{
	struct epoll_event ev;

	(*this).epollfd = epoll_create1(0);
	if ((*this).epollfd == -1)
	{
		std::cerr << "Error creating epoll" << std::endl;
		return -1;
	}
	ev.events = EPOLLIN;
	ev.data.fd = (*this).sockfd;
	if (epoll_ctl((*this).epollfd, EPOLL_CTL_ADD, (*this).sockfd, &ev) == -1)
	{
		std::cerr << "Error epoll_ctl" << std::endl;
		return -1;
	}
	return (0);
}

int Server::newClient()
{
	struct epoll_event ev;
	Client client;
	int conn_sock;

	conn_sock = client.acceptConnection((*this).sockfd);
	(*this).clients[conn_sock] = client;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = conn_sock;
	if (epoll_ctl((*this).epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
	{
		std::cerr << "Error epoll_ctl" << std::endl;
		return -1;
	}
	std::string msg = "CAP * LS :JOIN NICK\r\n";
	std::cout << "Client connected" << std::endl;
	send(conn_sock, msg.c_str(), msg.size(), 0);
	return (0);
}

void Server::start()
{
	struct epoll_event events[10];
	int nfds;

	if ((*this).init() == -1)
		return;
	if ((*this).initPoll() == -1)
		return;
	for (;;)
	{
		nfds = epoll_wait((*this).epollfd, events, 10, -1);
		if (nfds == -1)
		{
			std::cerr << "Error epoll_wait" << std::endl;
			return;
		}
		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == (*this).sockfd)
			{
				if ((*this).newClient() == -1)
					return ;
			}
			else
				(*this).execReq(&(*this).clients[events[n].data.fd]);
		}
	}
}

// PARSING PART
std::vector<std::string> extract_cmd(std::string req)
{
	std::vector<std::string> argv;
	std::string cursor(req);
	long unsigned int bnpos = -1;

	if (req[0] == ':')
	{
		cursor = req.substr(1);
	}
	for (long unsigned int spos = cursor.find(' ');; spos = cursor.find(' '))
	{
		bnpos = cursor.find('\r');
		argv.push_back(cursor.substr(0, (cursor.find(' ') == std::string::npos || cursor[0] == ':') ? bnpos : spos));
		if (cursor.find(' ') == std::string::npos || cursor[0] == ':')
			break;
		cursor = cursor.substr(spos + 1);
	}
	return argv;
}

void Server::execCmd(Client *client, std::vector<std::string> argv)
{
	std::string msg;
	std::string nick;
	if (argv.size() < 1)
		return;
	if (argv[0] == "NICK")
	{
		if (argv.size() != 2 || !(*client).getIsAuth())
			return;

		nick = argv[1];
		if ((*this).getClient(nick) != NULL)
		{
			msg = ":localhost 433 * " + nick + " :Nickname is already in use\r\n";
			(*client).sendMsg(msg);
			return ;
		}
		if ((*client).getIsNew())
		{
			msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".\r\n";
			(*client).setIsNew(false);
		}
		else
			msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost NICK :" + nick + "\r\n";
		(*client).setNick(nick);
		(*client).sendMsg(msg);
	}
	else if (argv[0] == "PASS")
	{
		if (argv[1] != (*this).password)
		{
			std::cout << (*client).getNick() << std::endl;
			msg = ":localhost 464 * :Incorrect Password\r\n";
			(*client).sendMsg(msg);
			(*client).disconnect((*this).epollfd);
			(*this).removeClient(client);
			return ;
		}
		(*client).setIsAuth(true);
	}
	else if (argv[0] == "JOIN")
	{
		if (argv.size() != 2 || argv[1] == ":")
			return;
		(*this).channels[argv[1]].addClient(client);
		msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost JOIN :" + argv[1] + "\r\n";
		(*this).channels[argv[1]].forwardMsg(client, msg);
		nick = "";
		for (int i = 0; i < (int)(*this).channels[argv[1]].getClients().size(); i++)
			nick += " " + (*(*this).channels[argv[1]].getClients()[i]).getNick();
		msg += ":server 353 " + (*client).getNick() + " = " + argv[1] + " :" + nick + "\r\n";
		msg += ":server 366 " + (*client).getNick() + " " + argv[1] + " :End of /NAMES list\r\n";
		
		(*client).sendMsg(msg);
	}
	else if (argv[0] == "PRIVMSG")
	{
		if (argv.size() != 3)
			return;
		msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost PRIVMSG " + argv[1] + " " + argv[2] + "\r\n";
		if (argv[1][0] == '#')
			(*this).channels[argv[1]].forwardMsg(client, msg);
		else
		{
			if ((*this).getClient(argv[1]))
				(*(*this).getClient(argv[1])).sendMsg(msg);
		}
	}
	else if (argv[0] == "KICK")
	{
		if (argv.size() != 4)
			return;
		msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost KICK " + argv[1] + " " + argv[2] + " " + argv[3] + "\r\n";
		for (int i = 0; i < (int)(*this).channels[argv[1]].getClients().size(); i++)
		{
			if ((*(*this).channels[argv[1]].getClients()[i]).getNick() == argv[2])
			{
				(*this).channels[argv[1]].forwardMsg(client, msg);
				(*client).sendMsg(msg);
				(*this).channels[argv[1]].removeClient((*this).channels[argv[1]].getClients()[i], "");
				break;
			}
		}
	}
	else if (argv[0] == "QUIT")
	{
		(*client).disconnect((*this).epollfd);
		(*this).removeClient(client);
	}
	else if (argv[0] == "PART")
	{
		if (argv.size() < 2)
			return ;
		msg = "PART " + argv[1];
		if (argv.size() > 2)
			msg += " " + argv[2];
		(*this).channels[argv[1]].removeClient(client, msg);
	}
	else
	{
		for (int i = 0; i < (int)argv.size(); i++)
			std::cout << argv[i] << " ";
		std::cout << std::endl;
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

std::string Server::readCmd(int const conn_sock)
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