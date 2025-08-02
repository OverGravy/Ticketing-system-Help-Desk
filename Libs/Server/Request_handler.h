// Server request handler header file, it contains the logic that handle the request riceved by the server

#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "../Tickets_list.h"
#include "./Login_handler.h"
#include "../Terminal_com.h"

int Request_handler(int client_fd, struct TicketNode* tickets_list, int buffer_size, struct AgentNode* agent_list); // Function that handle the request from the client

#endif