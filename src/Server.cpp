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

Server::Server(void)
{
	std::cout << "Server: Default constructor called." << std::endl;
	return ;
}

Server::Server(Server const &instance)
{
	std::cout << "Server: Copy constructor called." << std::endl;
	*this = instance;
	return ;
}

Server&	Server::operator=(Server const &instance)
{	
	std::cout << "Server: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

Server::~Server(void)
{	
	std::cout << "Server: Destructor called." << std::endl;
	return ;
}
