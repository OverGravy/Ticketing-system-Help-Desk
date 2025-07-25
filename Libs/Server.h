// Server side header file

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../Libs/Tickets_list.h"
#include "../Libs/Server_login.h"

#define DEFAULT_PROTOCOL 0
#define DEFAULT_PORT    1025

int server_start(int port);     // funcion that starts the server opening a socket and return it 

int server_loop(int socket);    // function that handle the server loop


#endif