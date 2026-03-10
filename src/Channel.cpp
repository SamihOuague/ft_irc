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
    (*this).clients.push_back(client);
}

std::vector<Client *>   Channel::getClients() const
{
    return (*this).clients;
}

Channel::~Channel(void)
{
	//std::cout << "Channel: Destructor called." << std::endl;
	return;
}
