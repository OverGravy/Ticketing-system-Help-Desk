#include "../../Libs/Server/Request_handler.h"

int Request_handler(int client_fd, struct TicketNode* tickets_list, int buffer_size, struct AgentNode* agent_list)
{
    struct Packet packet; // Create a packet to hold the ticket data
    int op_result; // Variable to hold the result of the operation

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
        op_result = add_ticket(tickets_list, packet.contenct);
        if (op_result == -1) {
            free(buffer);
            perror("Server: Error adding ticket to the list\n");
            return -1; // Error adding ticket
        } else {
            printf("Server: Ticket added successfully\n");
            
        }
    break;
    case 6: // request to sign in
        op_result = Singing_in(packet.contenct.agent_id, agent_list);
        if (op_result == -1) {
            free(buffer);
            perror("Server: Error signing in\n");
            return -1;
        }else if (op_result == 0) {
            free(buffer);
            printf("Server: Agent already signed in\n");
            return 0; // Agent already signed in, no need to proceed
        }
        else {
            printf("Server: Agent signed in successfully\n");
            // Send the key back to the client
            if (write(client_fd, &op_result, sizeof(op_result)) < 0) {
                free(buffer);
                perror("Server: Error sending key to client\n");
                return -1; // Error sending key
            }
        }
    break;
   default:
        printf("Server: Unable to reconigze the request type\n");
        return -1;                                              // the server is unable to handle the request
    break;
   }
    
   free(buffer); // Free the allocated buffer
   return 0; // Return 0 to indicate success
}
