#include "../../Libs/Server/Ticket_pile.h"

void init_ticket_pile(TicketPile* pile) {
    if (!pile) return;
    pile->tickets = NULL;
    pile->count = 0;
    pile->capacity = 0;
}

int push_new_ticket(TicketPile* pile, const Ticket* ticket) {
    if (!pile || !ticket) return -1;

    // Check if we need to resize (grow)
    if (pile->count >= pile->capacity) {
        int new_capacity = (pile->capacity == 0) ? 4 : pile->capacity * 2;
        Ticket* new_tickets = realloc(pile->tickets, new_capacity * sizeof(Ticket));
        if (!new_tickets) {
            return -1; // Memory allocation failed
        }
        pile->tickets = new_tickets;
        pile->capacity = new_capacity;
    }

    // Add the new ticket
    pile->tickets[pile->count++] = *ticket;
    return 0; // Success
}

int pop_ticket(TicketPile* pile, Ticket* out_ticket) {
    if (!pile || pile->count == 0) {
        return -1; // Invalid pile or empty pile
    }

    // Retrieve the last ticket
    *out_ticket = pile->tickets[--pile->count];

    // Check if we should shrink capacity
    if (pile->count > 0 && pile->count < pile->capacity / 4) {
        int new_capacity = pile->capacity / 2;
        if (new_capacity < 4) new_capacity = 4; // Avoid shrinking too much
        Ticket* new_tickets = realloc(pile->tickets, new_capacity * sizeof(Ticket));
        if (new_tickets) { // Only shrink if realloc succeeds
            pile->tickets = new_tickets;
            pile->capacity = new_capacity;
        }
    }

    return 0; // Success
}

void free_ticket_pile(TicketPile* pile) {
    if (!pile) return;
    free(pile->tickets);
    pile->tickets = NULL;
    pile->count = 0;
    pile->capacity = 0;
}

// Utility functions
int is_ticket_pile_empty(const TicketPile* pile) {
    return (!pile || pile->count == 0);
}

int ticket_pile_size(const TicketPile* pile) {
    return pile ? pile->count : 0;
}
