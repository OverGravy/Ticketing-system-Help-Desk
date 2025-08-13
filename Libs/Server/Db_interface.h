// header file contaning the functions needed to the server to create and operate the database
#ifndef TICKETS_LIST_H  
#define TICKETS_LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sqlite3.h>

#include "../Packets.h"

// function that init the database and create the tables that the ticketing system require
int init_db(sqlite3 **db, char* db_path);

// function that adds a ticket inside the db, return 0 if everithing went smooth, in case of an error return 0
int add_ticket(sqlite3 *db, const Ticket *ticket);

// function taht add an agenent redcond with his id and his password
int add_agent(sqlite3 *db, int agent_id, int pwd);

// funtion that retrive an agent password searching for his id 
int get_agent_password(sqlite3 *db, int agent_id, int *pwd_out);

// funtion that find a specific ticket inserted in the db
int get_ticket(sqlite3 *db, const TicketQuery *query, Ticket *result);

// function that find a ticket and modifies it, retruns 0 in case everithing went wrong, other wise the sql_error
int get_ticket_and_mod(sqlite3 *db, const TicketQueryAndMod* query_and_mod);

// function that close the database correcly
int close_database(sqlite3 *db);

#endif