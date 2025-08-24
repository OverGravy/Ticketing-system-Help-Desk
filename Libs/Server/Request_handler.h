// this header file contains different functions that handle all the different type of request
// REQ_SIGNIN,                   
// REQ_CREATE_TICKET,             
// REQ_QUERY_AND_MOD

#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <time.h>
#include <sqlite3.h>
#include "./Ticket_pile.h"
#include "./Db_interface.h"
#include "./Server_net_com.h"
#include "../Terminal_com.h"

// this header file contains different function that handle all the different type of request
// REQ_SIGNIN,                   
// REQ_CREATE_TICKET,             
// REQ_QUERY_AND_MOD,
// REQ_QUERY

// function that handle adding a ticket
// it sets the ticket id, status and date
void request_add_ticket(sqlite3* db, int ticket_count, RequestPacket* request, ResponsePacket* response);

// function that handle sing-in
// it checks if the client is already signed in and if not it signs him in
void request_sing_in(sqlite3* db, RequestPacket* request, ResponsePacket* response);

// function that handle client query
// it fills the response with all the tickets found and put them inside the pile passed as argument 
void request_client_query(sqlite3* db, RequestPacket* request, ResponsePacket* response, TicketPile* pile);

// function that handle the agent query
// it checks if the agent is logged in and if so it modifies the ticket if found and its unique
void request_agent_query(sqlite3* db, RequestPacket* request, ResponsePacket* response);


#endif