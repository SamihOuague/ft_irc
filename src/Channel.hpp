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
		std::vector<Client *> limitedUsers;
        std::string name;
		std::string	topic;
		std::string	pass;
		bool	inviteOnly;
		bool	topicRestricted;
		std::vector<std::string>	invited;

	public:
		Channel();
		Channel(Channel const &);
		Channel &	operator=(Channel const &);
        void addClient(Client *client);
        void removeClient(Client *client);
        std::vector<Client *> getClients() const;
		void	forwardMsg(Client *client, std::string &msg);
		bool	isOperator(Client *) const;
		void	addOperator(Client *);
		void	removeOp(Client *);
		Client	*getClient(std::string) const;
		bool	isLimited(std::string) const;
		void	limitUser(Client *);
		void	addUserLimit(Client *);
		void	removeUserLimit(Client *);
		void	addOp(Client *, Client *);
		void	setName(std::string);
		std::string	getName();
		std::string	getTopic(void) const;
		void	setTopic(std::string);
		bool	checkPass(std::string) const;
		void	setPass(std::string);
		void	setInviteOnly(bool);
		void	setTopicRestricted(bool);
		bool	isInviteOnly() const;
		bool	isTopicRestricted() const;
		bool	isInvited(std::string) const;
		void	addInvite(std::string);
		void	removeInvite(std::string nick);
		~Channel();
};
#endif
