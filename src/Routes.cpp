/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Routes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:54:36 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/14 09:22:43 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Routes.hpp"
#include <iostream>

Routes::Routes(void)
{
	//std::cout << "Routes: Default constructor called." << std::endl;
	return ;
}

Routes::Routes(Routes const &instance)
{
	//std::cout << "Routes: Copy constructor called." << std::endl;
	*this = instance;
	return ;
}

Routes&	Routes::operator=(Routes const &instance)
{	
	//std::cout << "Routes: Assignation overload called." << std::endl;
	if (this == &instance)
		return (*this);
	return (*this);
}

void	Routes::route(std::string path, void (*f)(Server *, Client *, std::vector<std::string>))
{
	(*this).routes[path] = f;
	return ;
}

Routes::~Routes(void)
{	
	//std::cout << "Routes: Destructor called." << std::endl;
	return ;
}
