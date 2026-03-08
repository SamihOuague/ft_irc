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

Client&	Client::operator=(Client const &instance)
{	
	std::cout << "Client: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

Client::~Client(void)
{	
	std::cout << "Client: Destructor called." << std::endl;
	return ;
}
