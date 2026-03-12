/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 13:37:50 by souaguen          #+#    #+#             */
/*   Updated: 2025/12/12 13:37:53 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <sys/socket.h>
# include <arpa/inet.h>
# include <string>
# include <vector>
#include <sys/epoll.h>

class	Client
{
	private:
		int	fd;
		sockaddr_in	addr;
		std::string	nick;
		std::string	user;
		std::string	password;
		bool	isNew;
		bool	isOperator;

	public:
		std::string	buffer;
	
		Client();
		Client(Client const &);
		Client &	operator=(Client const &);
		int		acceptConnection(int const &);
		int		getFd(void) const;
		std::string	getNick(void) const;
		void	setNick(std::string const &);
		std::string	getUser(void) const;
		void	setUser(std::string const &);
		std::string	getPassword(void) const;
		std::string	getPrefix(void) const;
		void	setPassword(std::string const &);
		bool	getIsNew(void) const;
		void	setIsNew(bool);
		void	sendMsg(std::string);
		void	setIsOperator(bool);
		bool	getIsOperator(void) const;
		void	disconnect(int &);
		~Client();
};
#endif