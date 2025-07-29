#ifndef __TICKET_H__
#define __TICKET_H__

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Define the structure to rappresent a date
struct Date {
    int day;
    int month;
    int year;
};

// Define the structure to represent a ticket
struct Ticket {    
   char title[20];
   char description[50];
   struct Date* creation_date;
   int priority;
   int status;                      // -1: closed 0: ongoing, 1: open
   int agent_id;                    // -1: no agent assigned
};

// Define the structure to represent a packet that will be sent over the network
struct Packet
{
   int type;   // this field provide information on what the oparation is 
   struct Ticket contenct;
};

// function that serialize a Ticket into a packet to be sent as a message
int serialize_packet(const struct Packet* p, char* buffer, size_t buf_size);

// function that deserialize a packet from a buffer
int deserialize_packet(const char* buffer, struct Packet* p);


#endif