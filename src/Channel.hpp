/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 04:00:40 by souaguen          #+#    #+#             */
/*   Updated: 2026/03/10 04:00:44 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Channel_HPP
# define Channel_HPP
# include <string>
# include <vector>
# include "Client.hpp"

class	Channel
{
	private:
		std::vector<Client *> clients;
		std::vector<Client *> ops;
        std::string name;

	public:
		Channel();
		Channel(Channel const &);
		Channel &	operator=(Channel const &);
        void addClient(Client *client);
        void removeClient(Client *client, std::string msg);
        std::vector<Client *> getClients() const;
		void	forwardMsg(Client *client, std::string &msg);
		bool	isOperator(Client *) const;
		void	removeOp(Client *);
		Client	*getClient(std::string) const;
		void	addOp(Client *);
		void	setName(std::string);
		~Channel();
};
#endif
