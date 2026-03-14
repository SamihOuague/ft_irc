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
# include "Routes.hpp"

class	Server: public Routes 
{
	private:
		sockaddr_in	addr;

	public:
        static bool	isRunning;
		int	epollfd;
		int	sockfd;
		std::string	password;
		std::string	opPassword;
		std::vector<std::string>	invited;
		std::map<int, Client> clients;
		std::map<std::string, Channel> channels;

		Server(void);
		Server(unsigned short, std::string);
		Server(Server const &);
		Server &	operator=(Server const &);
		~Server();
		void	start();
		int	readCmd(Client *);
		void	execCmd(Client *, std::vector<std::string>);
		void	execReq(Client *);
		int		init();
		int		initPoll();
		int		newClient();
		Client	*	getClient(std::string &);
		void	removeClient(Client *client);
		void	forwardMsg(std::string &msg);
};
#endif