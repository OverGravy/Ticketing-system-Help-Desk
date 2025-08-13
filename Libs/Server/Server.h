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

#include "./Request_handler.h"

#define DEFAULT_PROTOCOL 0
#define DEFAULT_PORT  1025

// funcion that starts the server opening a socket and return the file descriptor
int server_start(int port);     

// function that handle the all the server operation
int server_loop(int server_fd);    

#endif