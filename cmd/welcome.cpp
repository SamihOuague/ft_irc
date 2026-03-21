#include "commands.hpp"

void welcome(Client *client, Server *server)
{
    std::string msg;
    std::string nick;

    if ((*client).getNick() == "*"
        || (*client).getUser() == "~"
        || (*client).getPassword().empty()
        || (*server).password.compare((*client).getPassword())
        || !(*client).getIsNew())
        return ;
    nick = (*client).getNick();
    msg = ":localhost 001 " + nick + " :Welcome to the server " + nick + ".";
    (*client).setIsNew(false);
    (*client).sendMsg(msg);
}