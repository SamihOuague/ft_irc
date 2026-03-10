/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:37:50 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/12 13:37:53 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <arpa/inet.h>
# include <map>
# include <vector>
# include "Client.hpp"
# include "Channel.hpp"

class	Server
{
	private:
		int	sockfd;
		sockaddr_in	addr;
		std::map<int, Client> clients;
		std::map<std::string, Channel> channels;

	public:
		Server();
		Server(Server const &);
		Server &	operator=(Server const &);
		~Server();
		void	start(int const);
		std::string	readCmd(int const);
		void	execCmd(Client *, std::vector<std::string>);
		void	execReq(Client *);
		void	forwardMsg(Client *, std::vector<Client *>, std::string &);
};
#endif
