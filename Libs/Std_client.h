#ifndef STD_CLIENT_H
#define STD_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int client_start( int port);

int client_loop(int id, int client_fd); // id identify witch client is writing on the terminal

#endif