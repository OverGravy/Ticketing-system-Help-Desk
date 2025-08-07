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

// Define the users possible roles
typedef enum {
    ROLE_CLIENT,
    ROLE_AGENT
} UserRole;


// Define the tickets possible status
typedef enum {
    STATUS_OPEN,
    STATUS_IN_PROGRESS,
    STATUS_CLOSED
} TicketStatus;


// Define possible request types
typedef enum {
    REQ_SIGNIN,                     // client send a sing-in request with his agent_id
    REQ_LOGIN,                      // client send a login request with his agent_id, pwd and role requested
    REQ_CREATE_TICKET,              // client send a request to create a new ticket with title, description, priority and client_id the status is set by the server withn the date
    REQ_QUERY_AND_MODIFY_TICKET,    // client send a request to query and modify a ticket with title, description, and client_d, new_priority, new_status and new_client_id
    REQ_GET_PRIORITY_TICKET,        // client send a request to get the priority of a ticket sending the title and his client_id
} RequestType;


// Define possible response types
typedef enum {
    RESP_SING_IN_OK,
    RESP_TICKET_OK,
    RESP_AUTH_REQUIRED,
    RESP_ERROR
} ResponseType;


// Define a structure that rappresent a request to sign in
typedef struct {
    int agent_id;
} SingInRequest;


// Define a structure that rappresent a specific request for login
typedef struct {
    int agent_id;             
    int pwd;                   
} LoginRequest;


// Define the structure to rappresent a ticket
typedef struct {
    int id;                          // the id will be assigned by the server
    char title[MAX_TITLE_LEN];
    char description[MAX_DESC_LEN];
    char date[11];                   // date in format dd/mm/yyyy will be put by the server
    int priority;
    TicketStatus status;
    int client_id;                    
} Ticket;


// Define a structure that rappresent a request to create a new ticket, fields rappresent my filters 
typedef struct {
    char title[MAX_TITLE_LEN];
    char description[MAX_DESC_LEN];
    int client_id;                  
    int new_priority;
    TicketStatus new_status;
    int new_client_id; 
} TicketQueryAndModification;


// Define a structure that rappresent a request to query a ticket used by the client
typedef struct {   
    int client_id;              
    char title[MAX_TITLE_LEN];
} TicketQuery;


// Define a structure that rappresent a request from the client to the server
typedef struct {
    RequestType type;
    UserRole role;
    int sender_id;
    union {
        SingInRequest signin;
        LoginRequest login;
        Ticket new_ticket;
        TicketQuery priority_query; // for query requests
        TicketQueryAndModification mod_query;
    } data;
} RequestPacket;


// Define a structure that rappresent a response from the server
typedef struct {
    ResponseType type;
    TicketStatus status_code; 
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