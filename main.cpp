/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 13:25:07 by souaguen          #+#    #+#             */
/*   Updated: 2026/03/08 14:10:55 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <iostream>
#include <stdlib.h>
#include "commands.hpp"
#include "src/Server.hpp"

void signal_handler(int signum)
{
    std::cout << "Signal received: " << signum << std::endl;
    Server::isRunning = false;
}

int main(int argc, char **argv)
{
    unsigned int port = 0;
    std::string password;

    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    port = atoi(argv[1]);
    password = std::string(argv[2]);
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Invalid parameter: port should be a non 0 numeric value between 1 and 65535" << std::endl;
        return(1);
    }
    if (password.empty())
    {
        std::cerr << "Invalid password: Password should not be empty." << std::endl;
        return(1);
    }
    try
    {
        Server server(port, std::string(argv[2]));
        server.route("NICK", nick);
        server.route("PASS", pass);
        server.route("JOIN", join);
        server.route("PRIVMSG", privmsg);
        server.route("KICK", kick);
        server.route("QUIT", quit);
        server.route("PART", part);
        server.route("USER", user);
        server.route("PING", ping);
        server.route("OPER", oper);
        server.route("INVITE", invite);
        server.route("TOPIC", topic);
        server.route("MODE", mode);
        signal(SIGINT, signal_handler);
        server.start();
    }
    catch(const std::string &e)
    {
        std::cerr << e << std::endl;
    }
    return (0);
}
