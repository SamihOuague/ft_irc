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

#include "src/Server.hpp"
#include <signal.h>
#include <iostream>

void signal_handler(int signum)
{
    std::cout << "Signal received: " << signum << std::endl;
    Server::isRunning = false;
}

void nick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string nick;
    std::string msg;

    if (argv.size() != 2 || !(*client).getIsAuth())
        return;

    nick = argv[1];
    if ((*server).getClient(nick) != NULL)
    {
        msg = ":localhost 433 * " + nick + " :Nickname is already in use\r\n";
        (*client).sendMsg(msg);
        return;
    }
    if ((*client).getIsNew())
    {
        msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".\r\n";
        (*client).setIsNew(false);
    }
    else
        msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost NICK :" + nick + "\r\n";
    (*client).setNick(nick);
    (*client).sendMsg(msg);
}

void pass(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() < 2)
        return;
    if (argv[1] != (*server).password)
    {
        std::cout << (*client).getNick() << std::endl;
        msg = ":localhost 464 * :Incorrect Password\r\n";
        (*client).sendMsg(msg);
        (*client).disconnect((*server).epollfd);
        (*server).removeClient(client);
        return;
    }
    (*client).setIsAuth(true);
    return;
}

void join(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;
    std::string nick;

    if (argv.size() != 2 || argv[1] == ":")
        return;
    (*server).channels[argv[1]].addClient(client);
    msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost JOIN :" + argv[1] + "\r\n";
    (*server).channels[argv[1]].forwardMsg(client, msg);
    nick = "";
    for (int i = 0; i < (int)(*server).channels[argv[1]].getClients().size(); i++)
        nick += " " + (*(*server).channels[argv[1]].getClients()[i]).getNick();
    msg += ":server 353 " + (*client).getNick() + " = " + argv[1] + " :" + nick + "\r\n";
    msg += ":server 366 " + (*client).getNick() + " " + argv[1] + " :End of /NAMES list\r\n";

    (*client).sendMsg(msg);
    return;
}

void privmsg(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() != 3)
        return;
    msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost PRIVMSG " + argv[1] + " " + argv[2] + "\r\n";
    if (argv[1][0] == '#')
        (*server).channels[argv[1]].forwardMsg(client, msg);
    else
    {
        if ((*server).getClient(argv[1]))
            (*(*server).getClient(argv[1])).sendMsg(msg);
    }
    return;
}

void kick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() != 4)
        return;
    msg = ":" + (*client).getNick() + "!" + (*client).getNick() + "@localhost KICK " + argv[1] + " " + argv[2] + " " + argv[3] + "\r\n";
    for (int i = 0; i < (int)(*server).channels[argv[1]].getClients().size(); i++)
    {
        if ((*(*server).channels[argv[1]].getClients()[i]).getNick() == argv[2])
        {
            (*server).channels[argv[1]].forwardMsg(client, msg);
            (*client).sendMsg(msg);
            (*server).channels[argv[1]].removeClient((*server).channels[argv[1]].getClients()[i], "");
            break;
        }
    }
    return;
}

void quit(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)argv;
    (*client).disconnect((*server).epollfd);
    (*server).removeClient(client);
    return;
}

void part(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;
    if (argv.size() < 2)
        return;
    msg = "PART " + argv[1];
    if (argv.size() > 2)
        msg += " " + argv[2];
    (*server).channels[argv[1]].removeClient(client, msg);
    return;
}

int main(void)
{
    Server server(6868, "pass");

    server.route("NICK", nick);
    server.route("PASS", pass);
    server.route("JOIN", join);
    server.route("PRIVMSG", privmsg);
    server.route("KICK", kick);
    server.route("QUIT", quit);
    server.route("PART", part);

    signal(SIGINT, signal_handler);
    server.start();
    return (0);
}
