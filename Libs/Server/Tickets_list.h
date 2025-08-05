#ifndef TICKETS_LIST_H  
#define TICKETS_LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Packets.h"

// Define the structure that represents a node of the linked list
struct TicketNode {
    Ticket ticket;          // the ticket data
    struct TicketNode* next;       // pointer to the next node in the list
};

// function to add a new ticket to the list, return 0 if the operation is successful, -1 if it fails
int add_ticket(struct TicketNode* head, Ticket new_ticket, int ticket_id);
 
// function that returns a vector of tickets created by a specific client
struct TicketNode** get_tickets_by_agent_id(struct TicketNode* head, int agent_id);

// function that get a specific ticket id from the list using the ticket title
Ticket* get_ticket_by_title(struct TicketNode* head, const char* title);

// function that pick the ticket with the highest priority return it and remove it from the list
Ticket* get_highest_priority_ticket(struct TicketNode** head);

// function that destroy the list of tickets
void destroy_tickets_list(struct TicketNode* head);

#endif