/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Routes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:37:50 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/12 13:37:53 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTES_HPP
# define ROUTES_HPP
# include <map>
# include <string>
# include <vector>

class Server;
class Client;

class	Routes
{
	protected:
		std::map<std::string, void (*)(Server *, Client *, std::vector<std::string>)>	routes;

	public:
		Routes();
		Routes(Routes const &);
		Routes &	operator=(Routes const &);
		void	route(std::string, void (*)(Server *, Client *, std::vector<std::string>));
		~Routes();
};
#endif