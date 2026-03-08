/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <souaguen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 13:25:07 by souaguen          #+#    #+#             */
/*   Updated: 2026/03/08 14:10:55 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "src/Server.hpp"

int     main(void)
{
    Server server;

    server.start(6667);
    /* int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int clientfd;
    socklen_t socklen;
    sockaddr_in caddr;
    sockaddr_in addr;

    (void)clientfd;
    
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(6667);
    
    if (sockfd == -1)
        return (1);

    std::cout << "Socket creation successful" << std::endl;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cout << errno << std::endl;
        return (2);
    }
    std::cout << "Bind successful" << std::endl;

    if (listen(sockfd, 5) == -1)
       return (3);    
    
    std::cout << "Server listening on port 6667" << std::endl;
    socklen = sizeof(caddr);
    clientfd = accept(sockfd, (struct sockaddr *)&caddr, &socklen);
    if (clientfd == -1)
      return(-1);
    std::cout << "Client connected from " << inet_ntoa(caddr.sin_addr) << ":" << ntohs(caddr.sin_port) << std::endl;
    for (char buf = '\0'; read(clientfd, &buf, 1); )
    {
        std::cout << buf;
        if (buf == '\n')
            break;
    }
    std::cout << std::endl;
    const char *welcome = ":ft_irc_server 001 alice :Welcome to the IRC network, alice\r\n";
    send(clientfd, welcome, strlen(welcome), 0);
    //
    //for (char buf = '\0'; read(clientfd, &buf, 1); )
    //    std::cout << buf;
    // Boucle principale pour lire et répondre au client
    char buf[512];
    while (true)
    {
        int n = read(clientfd, buf, sizeof(buf) - 1);
        if (n <= 0) break; // client déconnecté
        buf[n] = '\0';
        std::cout << buf; // affiche la commande reçue
        // Pour test, renvoyer le message au client (echo)
        send(clientfd, buf, n, 0);
    }
    close(sockfd);
    close(clientfd);*/
    return (0);
}
