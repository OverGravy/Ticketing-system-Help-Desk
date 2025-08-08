#include "../../Libs/Server/Tickets_list.h"

int add_ticket(struct TicketNode **head, Ticket new_ticket, int ticket_id)
{

    // Allocate memory for the new node
    struct TicketNode *new_node = (struct TicketNode *)malloc(sizeof(struct TicketNode));

    // check if memory allocation was successful
    if (!new_node)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    new_node->ticket = new_ticket; // Copy the ticket data into the new node

    // ticket info that server need to add
    new_node->ticket.id = ticket_id;
    // new_node->ticket.date = "12/12/2012";
    new_node->ticket.status = STATUS_OPEN; // by default is open

    new_node->next = NULL; // Initialize the next pointer to NULL

    // If the list is empty, set the new node as the head
    if (*head == NULL)
    {
        *head = new_node; // modifica il valore originale
        printf("yo\n");
        return 0;
    }

    // Otherwise, find the end of the list and append the new node
    struct TicketNode *current = *head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node; // Append the new node at the end of the list

    printf("Aggiunta ticket:\n"
       "  ID: %d\n"
       "  Titolo: %s\n"
       "  Descrizione: %s\n"
       "  Data: %s\n"
       "  Priorità: %d\n"
       "  Stato: %d\n"
       "  ID Client: %d\n",
       new_ticket.id,
       new_ticket.title,
       new_ticket.description,
       new_ticket.date,
       new_ticket.priority,
       new_ticket.status,
       new_ticket.client_id);

    return 0;
}

// function that find a specific ticket inside the list considering the filters passed
bool match_ticket(Ticket *ticket, TicketQuery *query)
{
    if (query->ticket_id != -1 && ticket->id != query->ticket_id)
        return false;

    if (query->client_id != -1 && ticket->client_id != query->client_id)
        return false;

    if (query->title[0] != '\0' && strstr(ticket->title, query->title) == NULL)
        return false;

    if (query->description[0] != '\0' && strstr(ticket->description, query->description) == NULL)
        return false;

    return true;
}


Ticket *find_ticket(struct TicketNode *head, TicketQuery *filters)
{
    struct TicketNode *current = head;

    while (current != NULL)
    {
        if (match_ticket(&current->ticket, filters))
        {
            return &current->ticket; // match trovato
        }
        current = current->next;
    }

    return NULL; // Nessun match trovato
}


int modify_ticket(struct TicketNode* head, TicketModification* mod){
     
}

void destroy_tickets_list(struct TicketNode *head)
{
    struct TicketNode *current = head;
    struct TicketNode *next_node;
    while (current != NULL)
    {
        next_node = current->next; // Store the next node
        free(current);             // Free the current node
        current = next_node;       // Move to the next node
    }
    head = NULL; // Set the head to NULL to avoid dangling pointer
}