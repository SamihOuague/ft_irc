#ifndef COMMANDS_HPP
# define COMMANDS_HPP
# include "Server.hpp"
# include "Client.hpp"
# include <iostream>

std::string rplTopic(Channel, std::string);
bool    checkParams(std::vector<std::string>, int, Client *);
void    welcome(Client *, Server *);
void    invite(Server *, Client *, std::vector<std::string>);
void    join(Server *, Client *, std::vector<std::string>);
void    kick(Server *, Client *, std::vector<std::string>);
void    mode(Server *, Client *, std::vector<std::string>);
void    topic(Server *, Client *, std::vector<std::string>);
void    nick(Server *, Client *, std::vector<std::string>);
void    pass(Server *, Client *, std::vector<std::string>);
void    privmsg(Server *, Client *, std::vector<std::string>);
void    quit(Server *, Client *, std::vector<std::string>);
void    ping(Server *, Client *, std::vector<std::string>);
void    oper(Server *, Client *, std::vector<std::string>);
void    user(Server *, Client *, std::vector<std::string>);
void    ping(Server *, Client *, std::vector<std::string>);
void    part(Server *, Client *, std::vector<std::string>);
std::string toLowerStr(std::string);
bool    isBadFormatChannelName(std::string);
void    err_AlreadyRegistered(Client *);
void    err_PasswdMismatch(Client *);
void    err_NoSuchChannel(Client *, std::string);
void    err_NotOnChannel(Client *, std::string);
void    err_ChanOPrivsNeeded(Client *, std::string);
void    err_NoSuchNick(Client *, std::string);
void    err_UserNotInChannel(Client *, std::string, std::string);
void    err_NeedMoreParams(Client *, std::string);
std::vector<std::string>    ftSplitCommas(std::string to_split);
bool    isClientInChannel(Channel &channel, Client *client);
bool    isErroneusNickname(std::string nick);
#endif