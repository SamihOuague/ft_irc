#include "commands.hpp"
#include <iostream>

bool    isChannelJoinValid(Server *server, Client *client, std::string  channel_name, std::string key)
{
    std::string msg;

    if (isBadFormatChannelName(channel_name))
    {
        err_NoSuchChannel(client, channel_name);
        return false;
    }
    channel_name = toLowerStr(channel_name);
    if ((*server).channels.count(channel_name) == 0)
    {
        (*server).channels[channel_name].setName(channel_name);
        return true;
    }
    Channel &channel = (*server).channels[channel_name];
    if (channel.getLimit() != -1 && (int)channel.getClients().size() >= channel.getLimit())
    {
        msg = ":localhost 471 " + (*client).getNick() + " " + channel_name + " :Cannot join channel (+l)";
        (*client).sendMsg(msg);
        return false;
    }
    if (channel.isInviteOnly() && !channel.isInvited((*client).getNick())) //Ajouter methodes : isInvited qui check si client est present dans InvitedClients
    {
        msg = ":localhost 473 " + (*client).getNick() + " " + channel_name + " :Cannot join channel (+i)";
        (*client).sendMsg(msg);
        return false;
    }
    if (!channel.checkPass("") && !channel.checkPass(key))
    {
        msg = ":localhost 475 " + (*client).getNick() + " " + channel_name + " :Cannot join channel (+k)";
        (*client).sendMsg(msg);
        return false;
    }
    return true;
}

void    joinChannel(Client *client, Channel &channel)
{
    std::string msg;
    std::string nick;

    msg = (*client).getPrefix() + "JOIN :" + channel.getName();
    if (channel.getClient((*client).getNick()) == NULL)
    {
        channel.addClient(client);
        channel.forwardMsg(client, msg);
    }
    msg += "\n" + rplTopic(channel, (*client).getNick()) + "\n";
    nick = "";
    for (int i = 0; i < (int) channel.getClients().size(); i++)
    {
        if (i > 0)
            nick += " ";
        if (channel.isOperator(channel.getClients()[i]))
            nick += "@" + (*channel.getClients()[i]).getNick();
        else
            nick += (*channel.getClients()[i]).getNick();
    }
    msg += ":localhost 353 " + (*client).getNick() + " = " + channel.getName() + " :" + nick + "\n";
    msg += ":localhost 366 " + (*client).getNick() + " " + channel.getName() + " :End of /NAMES list";
    (*client).sendMsg(msg);
    channel.removeInvite((*client).getNick()); // Ajouter removeInvite a channel qui remove un client de InvitedClients
}

void join(Server *server, Client *client, std::vector<std::string> argv) 
{
    std::vector<std::string>    channel_names;
    std::vector<std::string>    keys;
    std::string                 key;
    unsigned int                channel_index;

    if (argv.size() < 2)
        return err_NeedMoreParams(client, "JOIN");
    channel_names = ftSplitCommas(argv[1]);
    if (argv.size() > 2)
        keys = ftSplitCommas(argv[2]);
    channel_index = 0;
    while(channel_index < channel_names.size())
    {
        key = "";
        if (channel_index < keys.size())
            key = keys[channel_index];
        if (!isChannelJoinValid(server, client, channel_names[channel_index], key))
        {
            channel_index++;
            continue;
        }
        joinChannel(client, (*server).channels[toLowerStr(channel_names[channel_index])]);
        channel_index++;
    }
    return;
}