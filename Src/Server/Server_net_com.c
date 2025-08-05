#include "../../Libs/Server/Server_net_com.h"

int request_handler(int client_fd, struct TicketNode* tickets_list, int buffer_size, struct AgentNode* agent_list)
{
    RequestPacket req;
    ResponsePacket resp;
    int op_result; // Variable to hold the result of the operation

    // Dinamic allocation of buffer cause i pass buffer_size as parameter
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        terminal_print(MSG_ERROR, "Error allocating memory for the buffer", SERVER, "Server");
        return -1;
    }

    // Read from the socket
    int character = read(client_fd, buffer, buffer_size - 1);
    if (character <= 0) {
        free(buffer);
        terminal_print(MSG_ERROR, "Error reading from socket", SERVER, "Server");
        return -1;
    }

    // Deserialize the packet using the dedicated function
    if (deserialize_request(buffer, &req) != 0) {
        free(buffer);
        terminal_print(MSG_ERROR, "Error deserializing the packet", SERVER, "Server");
        return -1;
    }

    // Process the request based on the request type
    switch (req.type)
    {
        case REQ_CREATE_TICKET: // request to add a ticket to the server
            op_result = add_ticket(tickets_list, req.data.new_ticket, 20);
            if (op_result == -1) {
                free(buffer);
                terminal_print(MSG_ERROR, "Error adding ticket", SERVER, "Server");
                return -1; // Error adding ticket
            } else {
                terminal_print(MSG_SUCCESS, "Ticket added successfully", SERVER, "Server");
            }
            break;

        case REQ_SIGNIN: // request to sign in
            op_result = Singing_in(req.data.signin.agent_id, agent_list);
            if (op_result == -1) {
                free(buffer);
                terminal_print(MSG_ERROR, "Error signing in agent", SERVER, "Server");
                return -1;
            } else if (op_result == 0) {
                free(buffer);
                terminal_print(MSG_INFO, "Agent already signed in", SERVER, "Server");
                return 0; // Agent already signed in, no need to proceed
            } else {
                terminal_print(MSG_SUCCESS, "Agent signed in successfully", SERVER, "Server");

                // prepare the response for the client
                resp.type = RESP_OK;
                resp.status_code = 0;
                snprintf(resp.message, sizeof(resp.message), "%d", op_result); // send the key inside the message

                if(send_response(client_fd, &resp) == -1){
                    terminal_print(MSG_ERROR, "Unable to send a response", SERVER, "Server");
                    free(buffer);
                    return -1;
                }

            }
            break;
        default:
            terminal_print(MSG_ERROR, "Unknown request type", SERVER, "Server");
            free(buffer);
            return -1; // the server is unable to handle the request
            break;
    }

    free(buffer); // Free the allocated buffer
    return 0; // Return 0 to indicat
}

int send_response(int client_fd, ResponsePacket* response_packet)
{
    char *resp_buffer = malloc(BUFFER_SIZE); // string to be sent as a response

    // Check if the memory for the buffer have been allocated correclty
    if (resp_buffer == NULL) {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Failed to allocate memory for response buffer", SERVER, "Server");
        return -1;
    }

    // Check if the response packet is valid
    if (response_packet == NULL) {
        terminal_print(MSG_ERROR, "Response packet is NULL", SERVER, "Server");
        return -1; // Error: response packet is NULL
    }

    // Serialize the response packet
    if (serialize_response(response_packet, resp_buffer) != 0) {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Error serializing response packet", SERVER, "Server");
        return -1; // Error serializing packet
    }

    // send the respone trought the socket
    if (write(client_fd, resp_buffer, strlen(resp_buffer)) == -1) {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Failed to write response to client", SERVER, "Server");
        return -1;
    }

    free(resp_buffer); // free the buffer after sending the response succesfully 

    return 0; // Response sent successfully
}
