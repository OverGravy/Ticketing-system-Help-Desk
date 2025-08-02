#include "../Libs/Tickets_list.h"


// fnction to add a new ticket to the list
int add_ticket(struct TicketNode* head, struct Ticket new_ticket){

    // Allocate memory for the new node
    struct TicketNode* new_node = (struct TicketNode*)malloc(sizeof(struct TicketNode));
    
    // check if memory allocation was successful
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    
    new_node->ticket = new_ticket; // Copy the ticket data into the new node
    new_node->next = NULL; // Initialize the next pointer to NULL
    
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



// function that returns all the tickets created by a specific agent
struct TicketNode** get_tickets_by_agent_id(struct TicketNode* head, int agent_id){

}



// function that get a specific ticket id from the list using the ticket title
struct Ticket* get_ticket_by_title(struct TicketNode* head, const char* title) {
    struct TicketNode* current = head;
    while (current != NULL) {
        if (strcmp(current->ticket.title, title) == 0) {
            return &current->ticket; // Return the ticket if the title matches
        }
        current = current->next; // Move to the next node
    }
    return NULL; // Return NULL if no ticket with the given title is found
}



// function that destroy the list of tickets
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