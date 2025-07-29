#include "../../Libs/Server/Request_handler.h"

int Request_handler(int client_fd, struct TicketNode *head, const int buffer_size)
{
    struct Packet packet; // Create a packet to hold the ticket data+

    // Dinamic allocation of buffer cause i pass buffer_size as parameter
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Server: Memory allocation failed for buffer\n");
        return -1;
    }

    // Read from the socket
    int character = read(client_fd, buffer, buffer_size - 1);
    if (character <= 0) {
        free(buffer);
        perror("Server: Error reading from socket or connection closed\n");
        return -1;
    }

    // Deserialize the packet using the dedicated function
    if (deserialize_packet(buffer, &packet) != 0) {
        free(buffer);
        perror("Server: Error deserializing packet\n");
        return -1;
    }

   switch (packet.type)
   {
   case 1: // request to add a ticket to the server
        add_ticket(head, packet.contenct);
    break;
   
   default:
        printf("Server: Unable to reconigze the request type");
        return -1;                                              // the server is unable to handle the request
    break;
   }
    
    return 0; // Return 0 to indicate success
}
