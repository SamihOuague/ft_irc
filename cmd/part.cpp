#include "commands.hpp"
#include <iostream>

void part(Server *server, Client *client, std::vector<std::string> argv)
{
    std::vector<std::string>    channel_names;
    std::string                 msg;
    std::string                 channel_name;
    unsigned int                channel_index;        

    if (argv.size() < 2)
        return err_NeedMoreParams(client, "PART");
    channel_names = ftSplitCommas(argv[1]);
    channel_index = 0;
    while(channel_index < channel_names.size())
    {
        channel_name = toLowerStr(channel_names[channel_index]);
        if ((*server).channels.count(channel_name) == 0)
        {
            err_NoSuchChannel(client, channel_name);
            channel_index++;
            continue;
        }
        Channel &channel = (*server).channels[channel_name];
        if (!isClientInChannel(channel, client))
        {
            err_NotOnChannel(client, channel_name);
            channel_index++;
            continue;
        }
        msg = (*client).getPrefix() + "PART " + channel_name;
        if (argv.size() > 2)
            msg += " " + argv[2];
        channel.forwardMsg(NULL, msg);
        channel.removeClient(client);
        if (channel.getClients().empty())
            (*server).channels.erase(channel_name);
        channel_index++;
    }
    return;
}