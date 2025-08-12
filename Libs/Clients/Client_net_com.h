#ifndef __CLIENT_COM__
#define __CLIENT_COM__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "../Packets.h"
#include "../Terminal_com.h"

// function that send a specific request packet to the server 
// return -1 if something goes wrong
int send_request(int server_fd, RequestPacket* req_packet, char* name);

//function that accept the response packet and popolate it from the socket 
// return -1 if something goes wrong
int accept_response(int server_fd, ResponsePacket* resp_packet, char* name);

// function that connects to the server every time a request need to be sent
int connect_to_server(int* server_fd, struct sockaddr_in *serv_addr);

#endif