// hedaer file that rappresnet a local ticket pile in the server obtained from the db
#ifndef TICKET_PILE_H
#define TICKET_PILE_H

#include "../Packets.h"
#include <string.h>
#include <stdlib.h>

// struct rappresenting a pile of tickets
typedef struct {
    Ticket* tickets; // Dynamic array of tickets
    int count;       // Number of tickets currently stored
    int capacity;    // Current capacity of the tickets array
} TicketPile;

// Initialize a TicketPile
void init_ticket_pile(TicketPile* pile);

// Add a ticket to the TicketPile, resizing if necessary
// Returns 0 on success, -1 on memory allocation failure
int push_new_ticket(TicketPile* pile, const Ticket* ticket);

// Remove and return the last ticket from the TicketPile
// Returns 0 on success, -1 if the pile is empty
int pop_ticket(TicketPile* pile, Ticket* out_ticket);

// Free the memory allocated for the TicketPile
void free_ticket_pile(TicketPile* pile);

#endif