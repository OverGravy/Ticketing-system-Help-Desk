#ifndef __SERVER_LOGIN_H__
#define __SERVER_LOGIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../Terminal_com.h"

// struct that memorizes the agent information
struct Agent {
    int client_id;
    int password;
};

// struct that is part of a list of agents
struct AgentNode {
    struct Agent agent;
    struct AgentNode* next;
};

// function that adds the agent to the list and create a key for the user and returns it, if the agent is already in the list it returns 0 in case of error returns -1
int Singing_in(int id, struct AgentNode* head);

int Logging_in(int id, struct AgentNode* head);

#endif