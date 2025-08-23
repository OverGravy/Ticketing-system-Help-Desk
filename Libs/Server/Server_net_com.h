// header file containing the functions needed by the server to comunicate using the socket
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
#include "./Authenitcation.h"
#include "../Packets.h"

// function that accept a request and rtaking the packet populateit
// it will deserialize the request
// it will return 0 in case everithing went right, -1 if something gose wrong 
int accept_request(int client_fd, RequestPacket* request_packet);

// Function that send a response to the client
// It will serialize the response packet and send it to the client
// It will return 0 if the response was sent successfully, -1 if there was an error
int send_response(int client_fd, ResponsePacket* response_packet);

#endif