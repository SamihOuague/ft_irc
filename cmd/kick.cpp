#include "commands.hpp"

void kick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::vector<std::string>    users_to_kick;
    std::string                 channel_name;
    std::string                 msg;
    std::string                 user_to_kick;
    unsigned int                users_to_kick_index;        

    if (argv.size() < 3)
        return err_NeedMoreParams(client, "KICK");
    channel_name = toLowerStr(argv[1]);
    if ((*server).channels.count(channel_name) == 0)
        return err_NoSuchChannel(client, argv[1]);
    Channel &channel = (*server).channels[channel_name];
    if (!isClientInChannel(channel, client))
        return err_NotOnChannel(client, channel_name);
    if (!channel.isOperator(client))
        return err_ChanOPrivsNeeded(client, channel_name);
    users_to_kick = ftSplitCommas(argv[2]);
    users_to_kick_index = 0;
    while(users_to_kick_index < users_to_kick.size())
    {
        user_to_kick = toLowerStr(users_to_kick[users_to_kick_index]);
        if ((*server).getClient(user_to_kick) == NULL)
        {
            err_NoSuchNick(client, user_to_kick);
            users_to_kick_index++;
            continue;
        }
        if (channel.getClient(user_to_kick) == NULL)
        {
            err_UserNotInChannel(client, user_to_kick, channel_name);
            users_to_kick_index++;
            continue;
        }
        msg = (*client).getPrefix() + "KICK " + channel_name + " " + user_to_kick;
        if (argv.size() > 3)
            msg += " " + argv[3];
        else
            msg += " :" + (*client).getNick();
        channel.forwardMsg(NULL, msg);
        channel.removeClient(channel.getClient(user_to_kick));
        if(channel.getClients().empty())
        {
            (*server).channels.erase(channel_name);
            break;
        }
        users_to_kick_index++;
    }
    return;
}