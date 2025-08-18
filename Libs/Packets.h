// header file that describe packets configuration.
// the application using 2 type of packets: request and response
// than this file implemnts the serialize and deserialize functions needed to send them trought the socket

#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TITLE_LEN 64
#define MAX_DESC_LEN 256
#define MAX_RESP_MSG_LEN 128
#define BUFFER_SIZE 1024          // buffer size for response and request


// define needed where a field is unused
#define INT_UNUSED -1
#define STR_UNUSED "-"

// Define the tickets possible status
typedef enum {
    STATUS_OPEN,
    STATUS_IN_PROGRESS,
    STATUS_CLOSED
} TicketStatus;


// Define possible request types
typedef enum {
    REQ_SIGNIN,                     // client send a sing-in request with his agent_id
    REQ_CREATE_TICKET,              // client send a request to create a new ticket with title, description, priority and client_id the status is set by the server withn the date
    REQ_QUERY,
    REQ_QUERY_AND_MOD
} RequestType;


// Define possible response types
typedef enum {
    RESP_SING_IN_OK,
    RESP_TICKET_OK,
    RESP_QUERY_OK,
    RESP_QUERY_MOD_OK,
    RESP_AUTH_REQUIRED,
    RESP_ERROR
} ResponseType;


// Define the structure to rappresent a ticket
typedef struct {
    int id;                          // the id will be assigned by the server
    char title[MAX_TITLE_LEN];
    char description[MAX_DESC_LEN];
    char date[11];                   // date in format dd/mm/yyyy will be put by the server
    int priority;                    // 0 if unsetted by the client
    TicketStatus status;             // the state will be open and put by the server as a default 
    int client_id;                    
} Ticket;


// Define a structure that rappresent a request to find a ticket, fields rappresents my filters 
typedef struct {
    int ticket_id;
    char title[MAX_TITLE_LEN];
    char description[MAX_DESC_LEN];
    int client_id;                  
} TicketQuery;


// Define a structure that rappresent a request to query a ticket used by the client
typedef struct {   
    int new_client_id;                  
    int new_priority;
    TicketStatus new_status;
} TicketModification;

// define a structure that be needed to be used by the agent to modify a ticket

typedef struct{
    int pwd;                     // password needed to log-in by the agent
    TicketQuery filters;         // filters used to search for the ticket inside the database
    TicketModification mod;      // field that i want to change 
} TicketQueryAndMod;

// Define a structure that rappresent a request from the client to the server
typedef struct {
    RequestType type;
    int sender_id;
    union {
        Ticket new_ticket;                    // possibility to add ticket accesable by the client
        TicketQuery Client_query;             // Query accessable by the client
        TicketQueryAndMod Agent_query;        // Query accessable by the agent
    } data;
} RequestPacket;


// Define a structure that rappresent a response from the server
typedef struct {
    ResponseType type;
    char message[MAX_RESP_MSG_LEN];            // free message for teh client
} ResponsePacket;

// function that serialize a RequestPacket into a buffer to be sent as a message
int serialize_request(const RequestPacket* req, char* buffer);

// function that deserialize a RequestPacket from a buffer
int deserialize_request(const char* buffer, RequestPacket* req);

// function that serialize a ResponsePacket into a buffer to be sent as a message
int serialize_response(const ResponsePacket* resp, char* buffer);

// function that deserialize a ResponsePacket from a buffer
int deserialize_response(const char* buffer, ResponsePacket* resp);

#endif