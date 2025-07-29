#ifndef __SERVER_LOGIN_H__
#define __SERVER_LOGIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Login {
    char username[50];
    char password[50];
};

struct Login_Node {
    struct Login login;
    struct Login_Node* next;
};

int Singing_in(int id);

int Logging_in(int id);

#endif