#include "commands.hpp"

void    rpl_invite(Server *server, Client *client)
{
    std::map<std::string, Channel>::iterator itc = (*server).channels.begin();
    std::string msg;

    for (int i = 0; i < (int)(*server).channels.size(); i++)
	{
        if ((*itc).second.isInvited((*client).getNick())) // isInvited a implementer
        {
            msg = ":localhost 336 " + (*client).getNick() + " " + (*itc).second.getName();
            (*client).sendMsg(msg);
        }
		itc++;
	}
    msg = ":localhost 337 " + (*client).getNick() + " :End of /INVITE list";
    (*client).sendMsg(msg);
}

void invite(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;
    std::string nick;
    std::string channel_name;

    if (argv.size() == 1)
        return rpl_invite(server, client);
    if (argv.size() == 2)
        return err_NeedMoreParams(client, "INVITE");
    nick = toLowerStr(argv[1]);
    if ((*server).getClient(nick) == NULL)
        return err_NoSuchNick(client, argv[1]);
    channel_name = toLowerStr(argv[2]);
    if ((*server).channels.count(channel_name) == 0)
        return err_NoSuchChannel(client, argv[2]);
    Channel &channel = (*server).channels[channel_name];
    if (!isClientInChannel(channel, client))
        return err_NotOnChannel(client, channel_name);
    if (channel.isInviteOnly() && !channel.isOperator(client))
            return err_ChanOPrivsNeeded(client, channel_name);
    if (channel.getClient(nick) != NULL)
    {
        msg = ":localhost 443 " + (*client).getNick() + " " + argv[1] + " " + channel_name + " :is already on channel";
        (*client).sendMsg(msg);
        return ;
    }
    msg = ":localhost 341 " + (*client).getNick()+ " " + argv[1]  + " " + channel_name;
    (*client).sendMsg(msg);
    msg = (*client).getPrefix() + "INVITE " + argv[1] + " " + channel_name;
    (*(*server).getClient(nick)).sendMsg(msg);
    if ((*server).getClient(nick))
        channel.addInvite(nick); //Ajouter vector de clients invites InvitedClients a Channel class
    return;
}