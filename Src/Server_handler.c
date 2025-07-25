#include "../Libs/Server_handler.h"

int Ticket_handler(int client_fd,struct TicketNode *head, const int buffer_size)
{
    char buffer[buffer_size];      // buffer needed to store the message
    int character;                 // number of char contained in the message
    struct Ticket t;               // new ticket to be added in the list 

    character = read(client_fd, buffer, buffer_size - 1);

    // error on reading
    if (character <= 0) {
         return -1;
    }

    buffer[character] = '\0'; // to use it as a string

   // 1. ID
    token = strtok(buffer, "|");
    if (token == NULL) return -1;
    t.id = atoi(token);  // converte stringa a intero

    // 2. Title
    token = strtok(NULL, "|");
    if (token == NULL) return -1;
    strncpy(t.title, token, sizeof(t.title));
    t.title[sizeof(t.title) - 1] = '\0'; // sicurezza

    // 3. Description
    token = strtok(NULL, "|");
    if (token == NULL) return -1;
    strncpy(t.description, token, sizeof(t.description));
    t.description[sizeof(t.description) - 1] = '\0';



    return 0;
}