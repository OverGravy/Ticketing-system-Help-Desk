// Server side header file with server logic 

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include "Request_handler.h"
#include "../Terminal_com.h"

#define DEFAULT_PROTOCOL 0
#define DEFAULT_PORT     1025
#define BUFFER_SIZE      1024

int server_start(int port);     // funcion that starts the server opening a socket and return it 

int server_loop(int server_fd);    // function that handle the server loop


#endif