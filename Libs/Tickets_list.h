#ifndef TICKETS_LIST_H  
#define TICKETS_LIST_H

#include <stdlib.h>
#include <stdio.h>

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

// Define the structure that represents a node of the linked list
struct TicketNode {
    struct Ticket ticket;          // the ticket data
    struct TicketNode* next;       // pointer to the next node in the list
};

// fnction to add a new ticket to the list
struct TicketNode* add_ticket(struct TicketNode* head, struct Ticket new_ticket);

// function that returns all the tickets created by a specific agent
struct TicketNode** get_tickets_by_agent_id(struct TicketNode* head, int agent_id);

// function that get a specific ticket id from the list using the ticket title
struct Ticket* get_ticket_by_title(struct TicketNode* head, const char* title);

// function that destroy the list of tickets
void destroy_tickets_list(struct TicketNode* head);


#endif