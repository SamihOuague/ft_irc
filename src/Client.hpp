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

class	Client
{
	public:
		Client();
		Client(Client const &);
		Client &	operator=(Client const &);
		~Client();
};
#endif
