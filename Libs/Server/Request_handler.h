// this header file contains different functions that handle all the different type of request
// REQ_SIGNIN,                   
// REQ_CREATE_TICKET,             
// REQ_QUERY_AND_MOD

#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "./Authenitcation.h"
#include "./Server_net_com.h"
#include "../Terminal_com.h"

// this header file contains different function that handle all the different type of request
// REQ_SIGNIN,                   
// REQ_CREATE_TICKET,             
// REQ_QUERY_AND_MOD,
// REQ_QUERY

// function that handle adding a ticket
int request_add_ticket(sqlite3* db, int ticket_count, RequestPacket* request, ResponsePacket* response);

// function that handle sing-in
int request_sing_in(sqlite3* db, RequestPacket* request, ResponsePacket* response);

// function that handle client query
int request_client_query(sqlite3* db, RequestPacket* request, ResponsePacket* response);

// function that handle the agent query
int request_agent_query(sqlite3* db, RequestPacket* request, ResponsePacket* response);


#endif