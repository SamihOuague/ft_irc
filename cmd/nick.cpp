#include "commands.hpp"

bool    isErroneusNickname(std::string nick)
{
    std::string::iterator   nick_index;

    if (nick.empty())
        return true;
    if (std::isdigit(nick[0]) || nick[0] == '$' || nick[0] == ':' || nick[0] == '#'
        || nick[0] == '&' || nick[0] == '+' || nick[0] == '!')
        return true;
    nick_index = nick.begin();
    while(nick_index != nick.end())
    {
        if (!(std::isalnum(*nick_index) || *nick_index == '_' || *nick_index == '\\'
            || *nick_index == '[' || *nick_index == ']' || *nick_index == '{'
            || *nick_index == '}' || *nick_index == '|' || *nick_index == '^'
            || *nick_index == '-'))
            return true;
        nick_index++;
    }
    return false;
}

void nick(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string nick;
    std::string msg;
    std::map<std::string, Channel>::iterator itc = (*server).channels.begin();

    if (argv.size() == 1)
    {
        msg = ":localhost 431 " + (*client).getNick() + " :No nickname given";
        (*client).sendMsg(msg);
        return;
    }
    nick = argv[1];
    if (isErroneusNickname(nick))
    {
        msg = ":localhost 432 " + (*client).getNick() + " " + nick + " :Erroneus nickname";
        (*client).sendMsg(msg);
        return;
    }
    nick = toLowerStr(nick);
    if ((*server).getClient(nick) != NULL)
    {
        msg = ":localhost 433 " + (*client).getNick() + " " + nick + " :Nickname is already in use";
        (*client).sendMsg(msg);
        return;
    }
    msg = (*client).getPrefix() + "NICK :" + nick;
    (*client).setNick(nick);
	for (int i = 0; i < (int)(*server).channels.size(); i++)
	{
        if (isClientInChannel((*itc).second, client))
		    (*itc).second.forwardMsg(client, msg);
		itc++;
	}
    (*client).sendMsg(msg);
    welcome(client, server);
}