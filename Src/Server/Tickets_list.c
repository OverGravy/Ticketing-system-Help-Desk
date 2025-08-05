#include "../../Libs/Server/Tickets_list.h"

int add_ticket(struct TicketNode* head, Ticket new_ticket, int ticket_id){

    // Allocate memory for the new node
    struct TicketNode* new_node = (struct TicketNode*)malloc(sizeof(struct TicketNode));
    
    // check if memory allocation was successful
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    
    new_node->ticket = new_ticket; // Copy the ticket data into the new node

    // ticket info that server need to add
    new_node->ticket.id = ticket_id;
    //new_node->ticket.date = "12/12/2012";
    new_node->ticket.status = STATUS_OPEN;  // by default is open

    new_node->next = NULL;          // Initialize the next pointer to NULL
    
    // If the list is empty, set the new node as the head
    if (head == NULL) {
        head = new_node; // Set the head to the new node
        return 0; // Return success
    }

    // Otherwise, find the end of the list and append the new node
    struct TicketNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node; // Append the new node at the end of the list

    return 0; 
}


struct TicketNode** get_tickets_by_agent_id(struct TicketNode* head, int agent_id){

}


Ticket* get_ticket_by_title(struct TicketNode* head, const char* title) {
    struct TicketNode* current = head;
    while (current != NULL) {
        if (strcmp(current->ticket.title, title) == 0) {
            return &current->ticket; // Return the ticket if the title matches
        }
        current = current->next; // Move to the next node
    }
    return NULL; // Return NULL if no ticket with the given title is found
}


void destroy_tickets_list(struct TicketNode* head) {
    struct TicketNode* current = head;
    struct TicketNode* next_node;
    while (current != NULL) {
        next_node = current->next; // Store the next node
        free(current); // Free the current node
        current = next_node; // Move to the next node
    }
    head = NULL; // Set the head to NULL to avoid dangling pointer
}