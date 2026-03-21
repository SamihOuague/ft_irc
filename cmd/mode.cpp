#include "commands.hpp"
#include <iostream>

bool    isValidKey(std::string key)
{
    size_t  key_index;

    if (key.empty())
        return false;
    key_index = 0;
    while(key_index < key.size())
    {
        if (key[key_index] == ' ' || key[key_index] == ',' || key[key_index] == '\0'
            || key[key_index] == '\r' || key[key_index] == '\n' || key[key_index] == '\a')
            return false;
        key_index++;
    }
    return true;
}

void    mode(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string flags;
    std::string vflags("");
    std::string msg;
    std::string flag_parameters;
    std::string nick;
    std::string channel_name;
    size_t      parameter_index;
    char        sign;

    if (argv.size() < 2)
        return err_NeedMoreParams(client, "MODE");
    if (argv[1][0] != '#' && argv[1][0] != '&' && argv[1][0] != '+' && argv[1][0] != '!')
    {
        std::cerr << "User mode not supported" << std::endl;
        return;
    }
    channel_name = toLowerStr(argv[1]);
    if ((*server).channels.count(channel_name) == 0)
        return err_NoSuchChannel(client, argv[1]);
    Channel &channel = (*server).channels[channel_name];
    if (argv.size() == 2)
    {
        msg = ":localhost 324 " + (*client).getNick() + " " + channel_name + " +" + channel.getModes(); // getModes a implementer dans Channel class qui renvoie en fonction des modes presents (+) sur le channel: "il" ou "itk" ou "itkl" etc... avec parametres apres pour 'l' ex: "itkl 10", pas de 'o' c'est specifique a un user
        (*client).sendMsg(msg);
        return;
    }
    if (!channel.isOperator(client))
        return err_ChanOPrivsNeeded(client, channel_name);
    flags = argv[2];
    parameter_index = 3;
    sign = flags[0];
    if (sign != '+' && sign != '-')
        sign = '+';
    msg = (*client).getPrefix() + "MODE " + channel_name + " " + sign;
    for (int i = 0; i < (int)flags.size(); i++)
    {
        if (flags[i] == 'i')
        {
            if (sign == '+' && !channel.isInviteOnly())
            {
                channel.setInviteOnly(true);
                vflags += 'i';
            }
            else if (sign == '-' && channel.isInviteOnly())
            {
                channel.setInviteOnly(false);
                vflags += 'i';
            }
        }
        else if (flags[i] == 't')
        {
            if (sign == '+' && !channel.isTopicRestricted())
            {
                channel.setTopicRestricted(true);
                vflags += 't';
            }
            else if (sign == '-' && channel.isTopicRestricted())
            {
                channel.setTopicRestricted(false);
                vflags += 't';
            }
        }
        else if (flags[i] == 'k' && (argv.size() > parameter_index || sign == '-'))
        {
            if (sign == '+' && !isValidKey(argv[parameter_index]))
            {
                (*client).sendMsg(":localhost 525 " + (*client).getNick() + " " + channel_name + " :Key is not well-formed");
                parameter_index++;
                continue;
            }
            if (sign == '+' && !channel.checkPass(argv[parameter_index]))
            {
                channel.setPass(argv[parameter_index]);
                vflags += 'k';
                flag_parameters += " " + argv[parameter_index];
            }
            else if (sign == '-' && !channel.checkPass(""))
            {
                channel.setPass("");
                vflags += 'k';
            }
            if (sign == '+')
                parameter_index++;
        }
        else if (flags[i] == 'o' && argv.size() > parameter_index)
        {
            nick = toLowerStr(argv[parameter_index]);
            if ((*server).getClient(nick) == NULL)
            {
                err_NoSuchNick(client, argv[parameter_index]);
                parameter_index++;
                continue;
            }
            if (channel.getClient(nick) == NULL)
            {
                err_UserNotInChannel(client, argv[parameter_index], channel_name);
                parameter_index++;
                continue;
            }
            if (sign == '+' && !channel.isOperator(channel.getClient(nick)))
            {
                channel.addOperator((*server).getClient(nick));
                vflags += 'o';
                flag_parameters += " " + argv[parameter_index];
            }
            if (sign == '-' && channel.isOperator(channel.getClient(nick)))
            {
                channel.removeOp((*server).getClient(nick));
                vflags += 'o';
                flag_parameters += " " + argv[parameter_index];
            }
            parameter_index++;
        }
        else if (flags[i] == 'l' && (argv.size() > parameter_index || sign == '-'))
        {
            if (sign == '+' && channel.getLimit() != atoi(argv[parameter_index].c_str()))
            {
                if (atoi(argv[parameter_index].c_str()) <= 0)
                    continue;
                channel.setLimit(atoi(argv[parameter_index].c_str()));
                vflags += 'l';
                flag_parameters += " " + argv[parameter_index];
            }
            if (sign == '-' && channel.getLimit() != -1)
            {
                channel.setLimit(-1);
                vflags += 'l';
            }
            if (sign == '+')
                parameter_index++;
        }
        else if ((flags[i] == '+' || flags[i] == '-') && flags[i] != sign)
        {
            sign = flags[i];
            vflags += sign;
        }
    }
    msg += vflags + flag_parameters;
    if (!vflags.empty())
        channel.forwardMsg(NULL, msg);
}