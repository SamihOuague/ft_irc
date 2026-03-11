/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:54:36 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/14 09:22:43 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <iostream>
#include <vector>

Channel::Channel(void)
{
	//std::cout << "Channel: Default constructor called." << std::endl;
	return;
}

Channel::Channel(Channel const &instance)
{
	//std::cout << "Channel: Copy constructor called." << std::endl;
	*this = instance;
	return;
}

Channel &Channel::operator=(Channel const &instance)
{
	//std::cout << "Channel: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

void Channel::addClient(Client *client)
{
	if ((*this).ops.size() == 0)
		(*this).addOp(client);
    (*this).clients.push_back(client);
}

void	Channel::removeOp(Client *client)
{
	std::vector<Client *>::iterator it = (*this).ops.begin();
	
	for (int i = 0; i < (int)(*this).ops.size(); i++)
	{
		if (*it == client)
		{ 
			(*this).ops.erase(it, it+1);
			break ;
		}
		it++;
	}
}

void	Channel::addOp(Client *client)
{
	std::string	msg = (*client).getPrefix() + "MODE " + (*this).name + " +o " + (*client).getNick();

	std::cout << msg << std::endl;
	(*this).removeOp(client);
	(*this).ops.push_back(client);
	(*this).forwardMsg(NULL, msg);
}

void	Channel::removeClient(Client *client, std::string msg)
{
	std::vector<Client *>::iterator it = (*this).clients.begin();
	std::string	msgi;
	
	msgi = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost " + msg;
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		if (*it == client)
		{
			if (msg.size() > 0)
				(*this).forwardMsg(NULL, msgi); 
			(*this).clients.erase(it, it+1);
			(*this).removeOp(client);
			break ;
		}
		it++;
	}
	if ((*this).ops.size() == 0)
		(*this).addOp(clients[0]);
}

Client	*Channel::getClient(std::string nick) const
{
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		if ((*(*this).clients[i]).getNick() == nick)
			return (*this).clients[i];
	}
	return (NULL);
}

void Channel::forwardMsg(Client *client, std::string &msg)
{
	if (client != NULL && (*this).getClient((*client).getNick()) == NULL)
		return ;
	for (int i = 0; i < (int)clients.size(); i++)
	{
		if ((*this).clients[i] != client)
			(*clients[i]).sendMsg(msg);
	}
}

std::vector<Client *>   Channel::getClients() const
{
    return (*this).clients;
}

bool	Channel::isOperator(Client *client) const
{
	for (int i = 0; i < (int)(*this).ops.size(); i++)
	{
		if ((*this).ops[i] == client)
			return (true);
	}
	return (false);
}

void	Channel::setName(std::string name)
{
	(*this).name = name;
}

Channel::~Channel(void)
{
	//std::cout << "Channel: Destructor called." << std::endl;
	return;
}
