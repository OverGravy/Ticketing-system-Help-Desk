// header file contaning the functions needed to the server to create and operate the database
#ifndef TICKETS_LIST_H  
#define TICKETS_LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sqlite3.h>

#include "../Packets.h"
#include "./Ticket_pile.h"

// function that init the database and create the tables that the ticketing system require
// return 0 if everithing went smothly, in case of an error it will return a negative number
int init_db(sqlite3 **db, char* db_path);

// function that adds a ticket inside the db
// return 0 if everithing went smooth, in case of an error return -1
int add_ticket(sqlite3 *db, const Ticket *ticket);

// function taht add an agenent record with his id and his password
// return 0 if everithing went smothly
// return a negative number in case of an error
// return 1 in case an agent with that id already exists
int add_agent(sqlite3 *db, int agent_id, int pwd);

// funtion that retrive an agent password searching for his id 
// return 0 if everithing went smothly, 
// return 1 in case no agent with that id was found
// return a negative number in case of an error
int get_agent_password(sqlite3 *db, int agent_id, int *pwd_out);

// funtion that find all the tickets that matches the query and put them inside the stack passed as argument
// funtion return the number of tickets found, in case of error it will return a negative number
int get_ticket(sqlite3 *db, const TicketQuery *query, TicketPile *pile);

// function that find a ticket and modifies it
// retrun 0 if everithing went smothly, in case of an error or no row found it will return a negative number
// retrun 1 in case it founds multiple tickets to modifiy thats noit allowed
// retrun 2 in case no ticket was found matching the query
int get_ticket_and_mod(sqlite3 *db, const TicketQueryAndMod* query_and_mod);

// function that close the database correcly
// return 0 if everithing went smothly, in case of an error it will return a negative number
int close_database(sqlite3 *db);

#endif