// header file containing functions used by th server to sing in an agent and the go-in the same agent
#ifndef __SERVER_LOGIN_H__
#define __SERVER_LOGIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../Terminal_com.h"
#include "./Db_interface.h"

// function that adds the agent to the list and create a password for the user and returns it
// return 0 if the agent already exist
// retrun -1 in case of an error
int Singing_in(sqlite3* db, int id);

// function that try to fin the agent id inside the database and cnfront the password in the table with the one givenì
// return 0 in case it matches
// retrun 1 if the password is wrong
// retrun -1 in case theres no agent with that id sing_in
// retreun -2 in case of an internal error
int Logging_in(sqlite3* db, int id, int pwd);

#endif