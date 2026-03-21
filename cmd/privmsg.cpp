#include "commands.hpp"

void privmsg(Server *server, Client *client, std::vector<std::string> argv)
{
    std::vector<std::string>    targets;
    std::string                 target;
    std::string                 msg;
    size_t                      target_index;

    if (argv.size() < 2)
    {
        msg = ":localhost 411 " + (*client).getNick() + " :No recipient given PRIVMSG";
        (*client).sendMsg(msg);
        return;
    }
    if (argv.size() < 3)
    {
        msg = ":localhost 412 " + (*client).getNick() + " :No text to send";
        (*client).sendMsg(msg);
        return;
    }
    targets = ftSplitCommas(argv[1]);
    target_index = 0;
    while(target_index < targets.size())
    {
        target = toLowerStr(targets[target_index]);
        msg = (*client).getPrefix() + "PRIVMSG " + target + " " + argv[2];
        if (target[0] != '#' && target[0] != '&' && target[0] != '+' && target[0] != '!')
        {
            if ((*server).getClient(target) == NULL)
            {
                err_NoSuchNick(client, target);
                target_index++;
                continue;
            }
            (*(*server).getClient(target)).sendMsg(msg);
        }
        else
        {
            if ((*server).channels.count(target) == 0)
            {
                err_NoSuchChannel(client, target);
                target_index++;
                continue;
            }
            if (!isClientInChannel((*server).channels[target], client))
            {
                 msg = ":localhost 404 " + (*client).getNick() + " " + target + " :Cannot send to channel";
                (*client).sendMsg(msg);
                target_index++;
                continue;
            }
            (*server).channels[target].forwardMsg(client, msg);
        }
        target_index++;
    }
    return;
}