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

void	Channel::setPass(std::string pass)
{
	(*this).pass = pass;
}

bool	Channel::checkPass(std::string pass) const
{
	if ((*this).pass.compare(pass) != 0)
		return false;
	return true;
}

void	Channel::addOperator(Client *client)
{
	if ((*this).isOperator(client))
		return ;
	(*this).ops.push_back(client);
}

bool	Channel::isInvited(std::string nick) const
{
	for (int i = 0; i < (int)(*this).invited.size(); i++)
	{
		if ((*this).invited[i] == nick)
			return (true);
	}
	return (false);
}

void	Channel::addInvite(std::string nick)
{
	if (!(*this).isInvited(nick))
		(*this).invited.push_back(nick);
}

void	Channel::removeInvite(std::string nick)
{
	std::vector<std::string>::iterator it = (*this).invited.begin();
	
	for (int i = 0; i < (int)(*this).invited.size(); i++)
	{
		if (*it == nick)
		{ 
			(*this).invited.erase(it, it+1);
			break ;
		}
		it++;
	}
}



bool	Channel::isLimited(std::string nick) const
{
	Client *user = (*this).getClient(nick);
	if (user == NULL)
		return false;
	for (int i = 0; i < (int)(*this).limitedUsers.size(); i++)
	{
		if ((*this).limitedUsers[i] == user)
			return (true);
	}
	return (false);
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

void	Channel::addUserLimit(Client *client)
{
	if (!(*this).isLimited((*client).getNick()))
		(*this).limitedUsers.push_back(client);
}

void	Channel::removeUserLimit(Client *client)
{
	std::vector<Client *>::iterator it = (*this).limitedUsers.begin();
	
	for (int i = 0; i < (int)(*this).limitedUsers.size(); i++)
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
	if ((*this).ops.size() == 0 && (*this).clients.size() != 0)
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

void	Channel::setInviteOnly(bool inviteOnly)
{
	(*this).inviteOnly = inviteOnly;
}

bool	Channel::isInviteOnly() const
{
	return (*this).inviteOnly;
}

void	Channel::limitUser(Client *client)
{
	(*this).limitedUsers.push_back(client);
}

void	Channel::setTopicRestricted(bool topicRestricted)
{
	(*this).topicRestricted = topicRestricted;
}

bool	Channel::isTopicRestricted() const
{
	return (*this).topicRestricted;
}