#ifndef STD_AGENT_H
#define STD_AGENT_H

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
#include <sys/wait.h>

#include "../../Packets.h"
#include "../../Terminal_com.h"
#include "../Client_net_com.h"
#include "./Agent_graphics.h"

#define DEFAULT_PORT 1025

// function that handle the main agent type client operation
int agent_client_loop(int id, int port, in_addr_t server_ip_addr);

#endif