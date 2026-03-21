#include "commands.hpp"

std::string toLowerStr(std::string str)
{
    for (int i = 0; i < (int)str.size(); i++)
        str[i] = std::tolower(str[i]);
    return str;
}

bool    isClientInChannel(Channel &channel, Client *client)
{
    for (int i = 0; i < (int) channel.getClients().size(); i++)
    {
        if (channel.getClients()[i] == client)
            return true;
    }
    return false;
}

std::vector<std::string>    ftSplitCommas(std::string to_split)
{
    std::vector<std::string>    splitted;
    size_t                      index;
    size_t                      delimiter_index;

    index = 0;
    delimiter_index = 0;
    while(index < to_split.size())
    {
        delimiter_index = to_split.find(',', index);
        if (delimiter_index == std::string::npos)
        {
            splitted.push_back(to_split.substr(index));
            break;
        }
        splitted.push_back(to_split.substr(index, delimiter_index - index));
        index = delimiter_index + 1;
    }
    return splitted;
}

bool    isBadFormatChannelName(std::string channel_name)
{
    if (channel_name[0] != '#' && channel_name[0] != '&' && channel_name[0] != '+'
        && channel_name[0] != '!')
        return true;
    if (channel_name.find(' ') != std::string::npos || channel_name.find(',') != std::string::npos
        || channel_name.find('\a') != std::string::npos)
        return true;
    return false;
}

void    err_AlreadyRegistered(Client *client)
{
    (*client).sendMsg(":localhost 462 " + (*client).getNick() + " :You may not reregister");
    return;
}

void    err_PasswdMismatch(Client *client)
{
    (*client).sendMsg(":localhost 464 " + (*client).getNick() + " :Incorrect Password");
    return;
}

void    err_NoSuchChannel(Client *client, std::string channel_name)
{
    (*client).sendMsg(":localhost 403 " + (*client).getNick() + " " + channel_name + " :No such channel");
    return;
}

void    err_NotOnChannel(Client *client, std::string channel_name)
{
    (*client).sendMsg( ":localhost 442 " + (*client).getNick() + " " + channel_name + " :You're not on that channel");
    return;
}

void    err_ChanOPrivsNeeded(Client *client, std::string channel_name)
{
    (*client).sendMsg(":localhost 482 " + (*client).getNick() + " " + channel_name + " :You're not channel operator");
    return;
}

void    err_NoSuchNick(Client *client, std::string nick)
{
    (*client).sendMsg(":localhost 401 " + (*client).getNick() + " " + nick + " :No such nick/channel");
    return;
}

void    err_UserNotInChannel(Client *client, std::string nick, std::string channel_name)
{
    (*client).sendMsg(":localhost 441 " + (*client).getNick() + " " + nick + " " + channel_name + " :They aren't on that channel");
    return;
}

void    err_NeedMoreParams(Client *client, std::string command)
{
    (*client).sendMsg(":localhost 461 " + (*client).getNick() + " " + command + " :Not enough parameters");
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

bool checkParams(std::vector<std::string> argv, int argc, Client *client)
{
    std::string msg = ":localhost 461 " + (*client).getNick() + " " + argv[0] + " :Not enough parameters";

    if ((int)argv.size() < argc)
        (*client).sendMsg(msg);
    return ((int)argv.size() >= argc);
}

void user(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)server;

    if (!checkParams(argv, 2, client))
        return;
    if (isErroneusNickname(argv[1]))
        return ;
    (*client).setUser(argv[1]);
    welcome(client, server);
    return;
}

void quit(Server *server, Client *client, std::vector<std::string> argv)
{
    (void)argv;
    (*client).disconnect((*server).epollfd);
    (*server).removeClient(client);
    return;
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