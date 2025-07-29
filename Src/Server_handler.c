#include "../Libs/Server_handler.h"

int Ticket_handler(int client_fd, struct TicketNode *head, const int buffer_size)
{
    // Dinamic allocation of buffer cause i pass buffer_size as parameter
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Ticket_handler: Memory allocation failed");
        return -1;
    }

    // Read from the socket
    int character = read(client_fd, buffer, buffer_size - 1);
    if (character <= 0) {
        free(buffer);
        perror("Ticket_handler: Error reading from socket or connection closed");
        return -1;
    }

    buffer[character] = '\0';

    struct Ticket t;
    char* token;

    // 1. Title
    token = strtok(buffer, ",");
    if (!token) {                 // every time the tok failed i have to free the buffer and return -1 as an error
        free(buffer);
        perror("Ticket_handler: Error parsing ticket title");
        return -1;
    }
    strncpy(t.title, token, sizeof(t.title) - 1);
    t.title[sizeof(t.title) - 1] = '\0';

    // 2. Description
    token = strtok(NULL, ",");
    if (!token) {
        free(buffer);
        perror("Ticket_handler: Error parsing ticket description");
        return -1;
    }
    strncpy(t.description, token, sizeof(t.description) - 1);
    t.description[sizeof(t.description) - 1] = '\0';

    // 3. Creation date
    token = strtok(NULL, ",");
    if (!token) {
        free(buffer);
        perror("Ticket_handler: Error parsing ticket creation date");
        return -1;
    }

    t.creation_date = malloc(sizeof(struct Date));
    if (t.creation_date == NULL) {                  // in case the malloc fail i have to free the buffer
        free(buffer);
        perror("Ticket_handler: Memory allocation for creation date failed");
        return -1;
    }
    sscanf(token, "%d/%d/%d", &t.creation_date->day, &t.creation_date->month, &t.creation_date->year);

    // 4. Priority
    token = strtok(NULL, ",");
    if (!token) {
        free(buffer);
        free(t.creation_date);
        perror("Ticket_handler: Error parsing ticket priority");
        return -1;
    }
    t.priority = atoi(token);

    // 5. Status
    token = strtok(NULL, ",");
    if (!token) {
        free(buffer);
        free(t.creation_date);
        perror("Ticket_handler: Error parsing ticket status");
        return -1;
    }
    t.status = atoi(token);

    add_ticket(head, t);  // Add the ticket to the linked list

    // buffer cleaning and return a positive code 
    free(buffer);
    return 0;
}
