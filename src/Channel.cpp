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

Channel::Channel(void): topic("")
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
	clients = instance.clients;
	ops = instance.ops;
    name = instance.name;
	topic = instance.topic;
	return (*this);
}

void Channel::addClient(Client *client)
{
	if ((*this).ops.size() == 0)
		(*this).addOp(client, client);
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

void	Channel::addOp(Client *client, Client *op)
{
	std::string	msg = (*op).getPrefix() + "MODE " + (*this).name + " +o " + (*client).getNick();

	(*this).removeOp(client);
	(*this).ops.push_back(client);
	(*this).forwardMsg(NULL, msg);
}

void	Channel::removeClient(Client *client)
{
	std::vector<Client *>::iterator it = (*this).clients.begin();
	
	for (int i = 0; i < (int)(*this).clients.size(); i++)
	{
		if (*it == client)
		{ 
			(*this).clients.erase(it, it+1);
			(*this).removeOp(client);
			break ;
		}
		it++;
	}
	if ((*this).ops.size() == 0)
		(*this).addOp(clients[0], client);
}

std::string	Channel::getName()
{
	return ((*this).name);
}

std::string	Channel::getTopic(void) const
{
	return ((*this).topic);
}

void	Channel::setTopic(std::string topic)
{
	(*this).topic = topic;
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
