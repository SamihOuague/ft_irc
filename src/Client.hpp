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

class	Client
{
	private:
		int	fd;
		int	socklen;
		sockaddr_in	addr;
		std::string username;

	public:
		Client();
		Client(Client const &);
		Client &	operator=(Client const &);
		int		acceptConnection(int const &);
		int		getFd(void) const;
		void	readCmd(void);
		~Client();
};
#endif