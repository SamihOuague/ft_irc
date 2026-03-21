#include "commands.hpp"

void    topic(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;
    std::string channel_name;

    if (argv.size() < 2)
        return err_NeedMoreParams(client, "TOPIC");
    channel_name = toLowerStr(argv[1]);
    if ((*server).channels.count(channel_name) == 0)
        return err_NoSuchChannel(client, argv[1]);
    Channel &channel = (*server).channels[channel_name];
    if (!isClientInChannel(channel, client))
        return err_NotOnChannel(client, channel_name);
    if (argv.size() > 2)
    {
        if (channel.isTopicRestricted() && !channel.isOperator(client))
            return err_ChanOPrivsNeeded(client, channel_name);
        channel.setTopic(argv[2]);
        msg = (*client).getPrefix() + "TOPIC " + channel_name + " " + argv[2];
        channel.forwardMsg(NULL, msg);
        return ;
    }
    msg = rplTopic(channel, (*client).getNick());
    (*client).sendMsg(msg);
    return ;
}