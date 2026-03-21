#include "Server.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

bool	Server::isRunning = false;

Server::Server(): Server(6667, "password")
{
	std::cout << "Server: Default constructor called." << std::endl;
	return;
}

Server::Server(unsigned short port, std::string password): password(password), opPassword("password")
{
	int opt = 1;

	(*this).sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if ((*this).sockfd == -1)
		throw "Error: opening socket";
	setsockopt((*this).sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	(*this).addr.sin_addr.s_addr = INADDR_ANY;
	(*this).addr.sin_family = AF_INET;
	(*this).addr.sin_port = htons(port);
}

Server::Server(Server const &instance): Routes()
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

void Server::forwardMsg(std::string &msg)
{
	std::map<int, Client>::iterator it = (*this).clients.begin();
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		(*it).second.sendMsg(msg);
		it++;
	}
}

void	Server::removeClient(Client *client)
{
	std::map<int, Client>::iterator it = (*this).clients.begin();
	std::map<std::string, Channel>::iterator itc = (*this).channels.begin();
	std::string	msg;

	for (int i = 0; i < (int)(*this).channels.size(); i++)
	{
		msg = (*client).getPrefix() + "PART : " + (*client).getNick() + " disconnected.";
		(*itc).second.removeClient(client);
		(*itc).second.forwardMsg(NULL, msg);
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
	(*this).opPassword = instance.opPassword;
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
		throw std::string("Error binding");
	std::cout << "Bind success" << std::endl;
	if (listen((*this).sockfd, 10) == -1)
		throw  std::string("Error listen");
	std::cout << "Server listening on port " << ntohs((*this).addr.sin_port) << std::endl;
	return (0);
}

int Server::initPoll()
{
	struct epoll_event ev;

	(*this).epollfd = epoll_create1(0);
	if ((*this).epollfd == -1)
		throw std::string("Error creating epoll");
	ev.events = EPOLLIN;
	ev.data.fd = (*this).sockfd;
	if (epoll_ctl((*this).epollfd, EPOLL_CTL_ADD, (*this).sockfd, &ev) == -1)
	{
		close((*this).sockfd);
		throw std::string("Error epoll_ctl");
	}
	return (0);
}

int Server::newClient()
{
	struct epoll_event ev;
	Client client;
	int conn_sock;

	try
	{
		conn_sock = client.acceptConnection((*this).sockfd);
	}
	catch(const std::string &e)
	{
		throw e;
	}
	(*this).clients[conn_sock] = client;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = conn_sock;
	if (epoll_ctl((*this).epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
		throw std::string("Error epoll_ctl");
	std::string msg = "CAP * LS :NICK PASS JOIN PRIVMSG KICK QUIT PART\r\n";
	std::cout << "Client connected" << std::endl;
	if (send(conn_sock, msg.c_str(), msg.size(), MSG_DONTWAIT) == -1)
		throw std::string("Sending to new client failed.");
	return (0);
}

void Server::start()
{
	struct epoll_event events[10];
	int nfds;

	Server::isRunning = true;
	try
	{
		(*this).init();
		(*this).initPoll();
	}
	catch (const std::string &e)
	{
		Server::isRunning = false;
		throw e;
	}
	while (Server::isRunning)
	{
		nfds = epoll_wait((*this).epollfd, events, 10, -1);
		if (nfds == -1)
		{
			if (errno == EINTR)
        		continue ;
			std::cerr << "Error epoll_wait" << std::endl;
			return;
		}
		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == (*this).sockfd)
			{
				try
				{
					(*this).newClient();
				}
				catch(const std::string &e)
				{
					std::cerr << e << std::endl;
				}
			}
			else
				(*this).execReq(&(*this).clients[events[n].data.fd]);
		}
	}
	
	std::map<int, Client>::iterator it = (*this).clients.begin();
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		(*it).second.disconnect((*this).epollfd);
		it++;
	}
	close((*this).sockfd);
	close((*this).epollfd);
}

// PARSING PART
std::vector<std::string> extract_cmd(std::string req)
{
	std::vector<std::string> argv;
	std::string cursor(req);
	std::string	tmp;
	long unsigned int bnpos = -1;

	bnpos = cursor.find('\r');
	if (bnpos != std::string::npos)
		cursor[bnpos] = '\n';
	for (long unsigned int spos = cursor.find(' ');; spos = cursor.find(' '))
	{
		while (cursor[0] == ' ')
		{
			cursor = cursor.substr(1);
			if (cursor[0] == '\n')
				return (argv);
		}
		bnpos = cursor.find('\n');
		spos = cursor.find(' ');
		bnpos = (cursor.find(' ') == std::string::npos || cursor[0] == ':') ? bnpos : spos;
		tmp = cursor.substr(0, bnpos);
		if (tmp != "")
			argv.push_back(tmp);
		if (cursor.find(' ') == std::string::npos || cursor[0] == ':')
			break ;
		cursor = cursor.substr(spos + 1);
	}
	return argv;
}

bool Server::execCmd(Client *client, std::vector<std::string> argv)
{
	std::string	msg = ":localhost 451 * :You have not registered";

	if (argv.size() < 1 || argv[0].empty() || argv[0] == "CAP")
		return (false);
	std::cout << argv[0] << " " << argv[1] << std::endl;
	if ((*this).routes.count(argv[0]) == 0) 
	{
		msg = ":localhost 421 " + argv[0] + " :Unknown command";
		(*client).sendMsg(msg);
		return (false);
	}
	if ((*client).getIsNew()
		&& argv[0] != "USER"
		&& argv[0] != "NICK"
		&& argv[0] != "PASS")
	{
		(*client).sendMsg(msg);
		return (false);
	}
	(*this).routes[argv[0]](this, client, argv);
	if (argv[0] == "QUIT")
		return (true);
	return (false);
}

void Server::execReq(Client *client)
{
	std::string cmd("");
	long unsigned int bnpos = -1;
	std::vector<std::string> argv;

	if ((*this).readCmd(client) == -1)
		return ;
	while ((*client).buffer.find('\n') != std::string::npos)
	{
		bnpos = (*client).buffer.find('\n');
		cmd = (*client).buffer.substr(0, bnpos);
		(*client).buffer = (*client).buffer.substr(bnpos + 1);
		argv = extract_cmd(cmd);
		if ((*this).execCmd(client, argv))
			break ;
	}
}

int	Server::readCmd(Client *client)
{
	char buffer[512];
	
	for (int bytes = recv((*client).getFd(), buffer, 511, MSG_DONTWAIT); ; bytes = recv((*client).getFd(), buffer, 511, MSG_DONTWAIT))
	{
		if (bytes <= 0)
		{
			if (bytes == 0)
			{
				(*client).disconnect((*this).epollfd);
				(*this).removeClient(client);
				return (-1);
			}
			return (0);
		} 
		buffer[bytes] = '\0';
		(*client).buffer += std::string(buffer);
	}
	return (0);
}