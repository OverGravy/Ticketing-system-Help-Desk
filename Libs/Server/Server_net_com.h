#ifndef SERVER_NET_H
#define SERVER_NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "../Terminal_com.h"
#include "./Tickets_list.h"
#include "./Login_handler.h"
#include "../Packets.h"

// Function that handle the reequest from the client
// It will read the request from the client, process it and send a response back
// It will return 0 if the request was processed successfully, -1 if there was an error
int request_handler(int client_fd, struct TicketNode* tickets_list, int buffer_size, struct AgentNode* agent_list); 

// Function that send a response to the client
// It will serialize the response packet and send it to the client
// It will return 0 if the response was sent successfully, -1 if there was an error
int send_response(int client_fd, ResponsePacket* response_packet);

#endif