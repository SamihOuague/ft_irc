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

void welcome(Client *client)
{
    std::string msg;
    std::string nick;

    if ((*client).getNick().empty()
            ||(*client).getUser().empty()
            || (*client).getPassword().empty()
            || !(*client).getIsNew())
        return ;
    nick = (*client).getNick();
    msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".";
    (*client).setIsNew(false);
    (*client).sendMsg(msg);
    
}



void nick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string nick;
    std::string msg;
    std::map<std::string, Channel>::iterator itc = (*server).channels.begin();

    if (argv.size() != 2)
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
    if (argv.size() < 2)
        return;
    (*client).setUser(argv[1]);
    
    welcome(client);
    return;
}

void pass(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() < 2)
        return;
    if (argv[1] != (*server).password)
    {
        std::cout << (*client).getNick() << std::endl;
        msg = ":localhost 464 * :Incorrect Password";
        (*client).sendMsg(msg);
        (*client).disconnect((*server).epollfd);
        (*server).removeClient(client);
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
    std::string msg;
    std::string nick;

    if (argv.size() != 2 || argv[1] == ":")
        return;
    if (argv[1][0] != '#')
        argv[1] = "#" + argv[1];
    if ((*server).channels.count(argv[1]) == 0)
        (*server).channels[argv[1]].setName(argv[1]);
    (*server).channels[argv[1]].addClient(client);
    msg = (*client).getPrefix() + "JOIN :" + argv[1] + "\n";
    (*server).channels[argv[1]].forwardMsg(client, msg);
    nick = "";
    for (int i = 0; i < (int)(*server).channels[argv[1]].getClients().size(); i++)
        nick += " " + (*(*server).channels[argv[1]].getClients()[i]).getNick();
    msg += ":localhost 353 " + (*client).getNick() + " = " + argv[1] + " :" + nick + "\n";
    msg += ":localhost 366 " + (*client).getNick() + " " + argv[1] + " :End of /NAMES list\n";
    msg += rplTopic((*server).channels[argv[1]], (*client).getNick());
    (*client).sendMsg(msg);
    return;
}

void privmsg(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() != 3)
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

    for (int i = 0; i < (int)argv.size(); i++)
        std::cout << argv[i] << " ";
    std::cout << std::endl;
    if (argv.size() != 4 && argv.size() != 3)
        return ;
    if (!(*server).channels[argv[1]].isOperator(client))
    {
        msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
        (*client).sendMsg(msg);
        return;
    }
    msg = (*client).getPrefix() + "KICK " + argv[1] + " " + argv[2];
    if (argv.size() == 4)
        msg += argv[3];
    msg += "\n" + (*(*server).getClient(argv[2])).getPrefix() + "PART " + argv[1];
    for (int i = 0; i < (int)(*server).channels[argv[1]].getClients().size(); i++)
    {
        if ((*(*server).channels[argv[1]].getClients()[i]).getNick() == argv[2])
        {
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
    (*client).disconnect((*server).epollfd);
    (*server).removeClient(client);
    return;
}

void part(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() < 2)
        return;
    msg = (*client).getPrefix() + "PART " + argv[1];
    if (argv.size() > 2)
        msg += " " + argv[2];
    (*server).channels[argv[1]].forwardMsg(NULL, msg);
    (*server).channels[argv[1]].removeClient(client);
    return;
}

void ping(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)server;
    if (argv.size() != 2)
        return;
    (*client).sendMsg("PONG :" + argv[1]);
}

void oper(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg = ":localhost 381 " + (*client).getUser() + " :You are now an IRC operator";
    std::string modeMsg = (*client).getPrefix() + "MODE " + (*client).getNick() + " :+o";

    if (argv.size() != 3)
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

    if (argv.size() != 3 || (*client).getNick() == argv[1])
        return;
    if ((*server).getClient(argv[1]) == NULL)
        return;
    msg = (*client).getPrefix() + "INVITE " + argv[1] + " :" + argv[2];
    std::cout << rplmsg << std::endl;
    std::cout << msg << std::endl;
    (*(*server).getClient(argv[1])).sendMsg(msg);
    (*client).sendMsg(rplmsg);
    (*server).channels[argv[2]].forwardMsg(NULL, msg);
    return ;
}

void    topic(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() != 2 && argv.size() != 3)
        return ;
    if (argv.size() == 3)
    {
        if (!(*server).channels[argv[1]].isOperator(client))
        {
            msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
            (*client).sendMsg(msg);
            return ;
        }
        (*server).channels[argv[1]].setTopic(argv[2]);
        msg = rplTopic((*server).channels[argv[1]], (*client).getNick());
        (*server).channels[argv[1]].forwardMsg(NULL, msg);
        return ;
    }
    msg = rplTopic((*server).channels[argv[1]], (*client).getNick());
    (*client).sendMsg(msg);
    return ;
}

/*
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel
*/

void    mode(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string flags;
    std::string msg;

    if (argv.size() < 3 || argv[1][0] != '#')
        return ;
    if (!(*server).channels[argv[1]].isOperator(client))
    {
        msg = ":localhost 482 " + argv[1] + " :You're not channel operator";
        (*client).sendMsg(msg);
        return;
    }
    flags = argv[2];
    if (flags[0] != '+' && flags[0] != '-')
        return ;
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
            if ((*server).getClient(argv[3]) == NULL
                    || (*server).channels[argv[1]].getClient(argv[3]) == NULL)
                continue ;
            if (flags[0] == '+')
                (*server).channels[argv[1]].addOperator((*server).getClient(argv[3]));
        }
        else if (flags[i] == 'l')
        {
            if ((*server).channels[argv[1]].getClient(argv[3]) == NULL)
                return ;
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
