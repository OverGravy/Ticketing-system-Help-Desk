#ifndef TICKETS_LIST_H  
#define TICKETS_LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../Packets.h"

// Define the structure that represents a node of the linked list
struct TicketNode {
    Ticket ticket;          // the ticket data
    struct TicketNode* next;       // pointer to the next node in the list
};

// function to add a new ticket to the list, return 0 if the operation is successful, -1 if it fails
int add_ticket(struct TicketNode** head, Ticket new_ticket, int ticket_id);
 
// function that find a specific ticket inside the list considering the filters passed
Ticket * find_ticket(struct TicketNode* head, TicketQuery* filters);

// function that modify a specific ticket with the field of the modification
int modify_ticket(struct TicketNode* head, TicketModification* mod);

// function that pick the ticket with the highest priority return it and remove it from the list
Ticket* get_highest_priority_ticket(struct TicketNode** head);

// function that destroy the list of tickets
void destroy_tickets_list(struct TicketNode* head);

#endif