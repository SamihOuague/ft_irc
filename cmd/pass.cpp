#include "commands.hpp"

void pass(Server *server, Client *client, std::vector<std::string> argv)
{
    std::string msg;

    if (argv.size() < 2)
        return err_NeedMoreParams(client, argv[0]); //Avant le 1er NICK, nick = "*" , a rajouter dans le constructeur de Client
    (*client).setPassword(argv[1]);   
    if (!(*client).getIsNew())
        return err_AlreadyRegistered(client);
    if (argv[1] != (*server).password)
    {
        err_PasswdMismatch(client);
        return ;
    }
    welcome(client, server);
    return;
}