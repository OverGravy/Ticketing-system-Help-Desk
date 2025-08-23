#include "../../Libs/Server/Server_net_com.h"

int accept_request(int client_fd, RequestPacket *request_packet)
{

    int character;

    char *req_buffer = malloc(BUFFER_SIZE); // string recived as request

    // check if req_buffer have been allocated correcly
    if (req_buffer == NULL)
    {
        terminal_print(MSG_ERROR, "Error allocating memory for the buffer", SERVER, "Server");
        return -1;
    }

    // read what camed trought the socket
    character = read(client_fd, req_buffer, BUFFER_SIZE - 1);
    if (character <= 0)
    {
        free(req_buffer);
        terminal_print(MSG_ERROR, "Error reading from socket", SERVER, "Server");
        return -1;
    }

    // Deserialize the packet using the dedicated function
    if (deserialize_request(req_buffer, request_packet) != 0)
    {
        free(req_buffer);
        terminal_print(MSG_ERROR, "Error deserializing the packet", SERVER, "Server");
        return -1;
    }

    return 0; // everithing went well
}

int send_response(int client_fd, ResponsePacket *response_packet)
{
    char *resp_buffer = malloc(BUFFER_SIZE); // string to be sent as a response

    // Check if the memory for the buffer have been allocated correclty
    if (resp_buffer == NULL)
    {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Failed to allocate memory for response buffer", SERVER, "Server");
        return -1;
    }

    // Check if the response packet is valid
    if (response_packet == NULL)
    {
        terminal_print(MSG_ERROR, "Response packet is NULL", SERVER, "Server");
        return -1; // Error: response packet is NULL
    }

    // Serialize the response packet
    if (serialize_response(response_packet, resp_buffer) != 0)
    {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Error serializing response packet", SERVER, "Server");
        return -1; // Error serializing packet
    }

    // send the respone trought the socket
    if (write(client_fd, resp_buffer, strlen(resp_buffer)) == -1)
    {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Failed to send response to client", SERVER, "Server");
        return -1;
    }

    free(resp_buffer); // free the buffer after sending the response succesfully

    return 0; // Response sent successfully
}
