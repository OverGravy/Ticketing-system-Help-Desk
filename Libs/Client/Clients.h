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

#include "../Ticket.h"
#include "../Terminal_com.h"

// function that starts the client side with the right server parameter
int client_start(int port, in_addr_t server_ip_addr);

int client_loop(int id, int client_fd); // id identify witch client is writing on the terminal

int agent_client_loop(int id, int client_fd); 


#endif