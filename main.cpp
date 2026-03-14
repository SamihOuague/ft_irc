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
#include <stdlib.h>

void signal_handler(int signum)
{
    std::cout << "Signal received: " << signum << std::endl;
    Server::isRunning = false;
}

void welcome(Client *client)
{
    std::string msg;
    std::string nick;

    if ((*client).getNick().empty()
        || (*client).getUser().empty()
        || (*client).getPassword().empty()
        || !(*client).getIsNew())
        return;
    nick = (*client).getNick();
    msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".";
    (*client).setIsNew(false);
    (*client).sendMsg(msg);
}

bool checkParams(std::vector<std::string> argv, int argc, Client *client)
{
    std::string msg = ":localhost 461 " + (*client).getNick() + " " + argv[0] + " :Not enough parameters";

    if ((int)argv.size() < argc)
        (*client).sendMsg(msg);
    return ((int)argv.size() >= argc);
}

void nick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string nick;
    std::string msg;
    std::map<std::string, Channel>::iterator itc = (*server).channels.begin();

    if (!checkParams(argv, 2, client))
        return;
    nick = argv[1];
    if ((*server).getClient(nick) != NULL)
    {
        msg = ":localhost 433 * " + nick + " :Nickname is already in use";
        (*client).sendMsg(msg);
        return;
    }

    msg = (*client).getPrefix() + "NICK :" + nick;
    (*client).setNick(nick);
    for (int i = 0; i < (int)(*server).channels.size(); i++)
    {
        (*itc).second.forwardMsg(client, msg);
        itc++;
    }
    (*client).sendMsg(msg);
    welcome(client);
}

void user(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)server;

    if (!checkParams(argv, 2, client))
        return;
    (*client).setUser(argv[1]);

    welcome(client);
    return;
}

void pass(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (!checkParams(argv, 1, client))
        return;

    if ((int)argv.size() == 1)
        argv.push_back("");
    if (argv[1] != (*server).password)
    {
        msg = ":localhost 464 * :Incorrect Password";
        (*client).sendMsg(msg);
        return;
    }
    (*client).setPassword(argv[1]);
    welcome(client);
    return;
}

std::string rplTopic(Channel channel, std::string nick)
{
    std::string rplmsg;

    rplmsg = ":localhost 332 " + nick + " " + channel.getName() + " " + channel.getTopic();
    if (channel.getTopic().empty())
        rplmsg = ":localhost 331 " + channel.getName() + " :No topic";
    return (rplmsg);
}

void join(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg = "";
    std::string util;
    std::string tmp;
    std::string chan;

    if (!checkParams(argv, 2, client))
        return;
    msg = ":localhost 473 " + argv[1] + " :Cannot join channel (+i)";
    if ((*server).channels.count(chan) != 0
            && !(*server).channels[chan].isInvited((*client).getNick()))
    {
        (*client).sendMsg(msg);
        return ;
    }
    tmp = argv[1];
    for (;;)
    {
        chan = tmp.substr(0, tmp.find(','));
        if (chan[0] != '#')
            chan = '#' + chan;
        msg += (*client).getPrefix() + "JOIN :" + chan + "\n";
        if ((*server).channels.count(chan) == 0)
            (*server).channels[chan].setName(chan);
        (*server).channels[chan].addClient(client);
        util = (*client).getPrefix() + "JOIN :" + chan;
        (*server).channels[argv[1]].forwardMsg(client, util);
        util = "";
        for (int i = 0; i < (int)(*server).channels[chan].getClients().size(); i++)
            util += " " + (*(*server).channels[chan].getClients()[i]).getNick();
        msg += ":localhost 353 " + (*client).getNick() + " = " + chan + " :" + util + "\n";
        msg += ":localhost 366 " + (*client).getNick() + " " + chan + " :End of /NAMES list\n";
        msg += rplTopic((*server).channels[chan], (*client).getNick()) + "\n";
        if (tmp.find(',') == std::string::npos)
            break;
        tmp = tmp.substr(tmp.find(',') + 1);
    }
    (*client).sendMsg(msg);
    return;
}

void privmsg(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (!checkParams(argv, 3, client))
        return;
    msg = (*client).getPrefix() + "PRIVMSG " + argv[1] + " " + argv[2];
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

    if (!checkParams(argv, 3, client))
        return;
    if (!(*server).channels[argv[1]].isOperator(client))
    {
        msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
        (*client).sendMsg(msg);
        return;
    }
    msg = (*client).getPrefix() + "KICK " + argv[1] + " " + argv[2];
    if (argv.size() == 4 && argv[3][0] == ':')
        msg += " " + argv[3];
    msg += "\n" + (*(*server).getClient(argv[2])).getPrefix() + "PART " + argv[1];
    for (int i = 0; i < (int)(*server).channels[argv[1]].getClients().size(); i++)
    {
        if ((*(*server).channels[argv[1]].getClients()[i]).getNick() == argv[2])
        {
            (*server).channels[argv[1]].removeInvite((*(*server).channels[argv[1]].getClients()[i]).getNick());
            (*server).channels[argv[1]].forwardMsg(NULL, msg);
            (*server).channels[argv[1]].removeClient((*server).channels[argv[1]].getClients()[i]);
            break;
        }
    }
    return;
}

void quit(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)argv;
    (*server).removeClient(client);
    (*client).disconnect((*server).epollfd);
    return;
}

void part(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;
    std::string chan;
    std::string util;
    std::string tmp;

    if (!checkParams(argv, 2, client))
        return;
    tmp = argv[1];
    std::cout << tmp << std::endl; 
    for (;;)
    {
        chan = tmp.substr(0, tmp.find(','));
        if (chan[0] != '#')
            chan = '#' + chan;
        msg = (*client).getPrefix() + "PART " + chan;    
        if ((*server).channels.count(chan) != 0)
        {
            if (argv.size() > 2)
                msg += " " + argv[2];
            (*server).channels[chan].forwardMsg(NULL, msg);
            (*server).channels[chan].removeClient(client);
        }
        if (tmp.find(',') == std::string::npos)
            break;
        tmp = tmp.substr(tmp.find(',') + 1);
    }
}

void ping(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)server;
    if (!checkParams(argv, 2, client))
        return;
    (*client).sendMsg("PONG :" + argv[1]);
}

void oper(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg = ":localhost 381 " + (*client).getUser() + " :You are now an IRC operator";
    std::string modeMsg = (*client).getPrefix() + "MODE " + (*client).getNick() + " :+o";

    if (!checkParams(argv, 3, client))
        return;
    if ((*client).getUser() != argv[1])
        msg = ":localhost 491 " + (*client).getUser() + " :No O-lines for your host";
    else if ((*server).opPassword != argv[2])
        msg = ":localhost 464 " + (*client).getUser() + " :Incorrect Password";
    else
    {
        (*client).setIsOperator(true);
        (*server).forwardMsg(modeMsg);
    }
    (*client).sendMsg(msg);
}

void invite(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string rplmsg = ":localhost 341 " + argv[2] + " " + (*client).getNick() + " :" + argv[1];
    std::string msg;

    if (!checkParams(argv, 3, client))
        return;
    if (argv[2][0] != '#')
        argv[2] = '#' + argv[2];
    if (!(*server).channels[argv[2]].isOperator(client))
    {
        msg = ":localhost 482 " + argv[2] + " :You're not channel operator";
        (*client).sendMsg(msg);
        return;
    }
    msg = (*client).getPrefix() + "INVITE " + argv[1] + " :" + argv[2];
    (*server).channels[argv[2]].addInvite((*client).getNick());
    (*(*server).getClient(argv[1])).sendMsg(msg);
    (*client).sendMsg(rplmsg);
    (*server).channels[argv[2]].forwardMsg(NULL, msg);
    return;
}

void topic(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (!checkParams(argv, 2, client))
        return;
    if (argv.size() == 3)
    {
        if (!(*server).channels[argv[1]].isOperator(client))
        {
            msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
            (*client).sendMsg(msg);
            return;
        }
        (*server).channels[argv[1]].setTopic(argv[2]);
        msg = rplTopic((*server).channels[argv[1]], (*client).getNick());
        (*server).channels[argv[1]].forwardMsg(NULL, msg);
        return;
    }
    msg = rplTopic((*server).channels[argv[1]], (*client).getNick());
    (*client).sendMsg(msg);
    return;
}

void mode(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string flags;
    std::string msg;

    if (argv.size() == 2)
        return;
    if (!checkParams(argv, 3, client) || (*server).channels.count(argv[1]) == 0)
        return;
    if (!(*server).channels[argv[1]].isOperator(client))
    {
        msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
        (*client).sendMsg(msg);
        return;
    }
    flags = argv[2];
    if (flags[0] != '+' && flags[0] != '-')
        return;
    msg = (*client).getPrefix() + "MODE " + argv[1] + " " + flags[0];
    for (int i = 1; i < (int)flags.size(); i++)
    {
        if (flags[i] == 'i')
            (*server).channels[argv[1]].setInviteOnly(flags[0] == '+');
        else if (flags[i] == 't')
            (*server).channels[argv[1]].setTopicRestricted(flags[0] == '+');
        else if (flags[i] == 'k' && (argv.size() == 4 || flags[0] == '-'))
            (*server).channels[argv[1]].setPass((flags[0] == '+') ? argv[3] : "");
        else if (flags[i] == 'o' && argv.size() == 4)
        {
            if ((*server).getClient(argv[3]) == NULL || (*server).channels[argv[1]].getClient(argv[3]) == NULL)
                continue;
            if (flags[0] == '+')
                (*server).channels[argv[1]].addOperator((*server).getClient(argv[3]));
        }
        else if (flags[i] == 'l')
        {
            if ((*server).channels[argv[1]].getClient(argv[3]) == NULL)
                return;
            if (flags[0] == '+')
                (*server).channels[argv[1]].addUserLimit((*server).getClient(argv[3]));
            else
                (*server).channels[argv[1]].removeUserLimit((*server).getClient(argv[3]));
        }
        if (std::string("itkol").find(flags[i]) != std::string::npos)
            msg += flags[i];
    }
    msg += (argv.size() == 4) ? argv[3] : "";
    (*server).channels[argv[1]].forwardMsg(NULL, msg);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    if (atoi(argv[1]) <= 0)
    {
        std::cout << "Invalid parameter: port should be a non 0 numeric value" << std::endl;
        return (1);
    }
    Server server(atoi(argv[1]), std::string(argv[2]));

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
    return (0);
}
