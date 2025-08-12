#include "../../Libs/Clients/Client_net_com.h"

int send_request(int server_fd, RequestPacket* req_packet, char* name){

    char* req_buffer = malloc(BUFFER_SIZE); 

    // chack the server fd
    if(server_fd == -1){
        terminal_print(MSG_ERROR, "server desciptor fiile not valid", CLIENT, name);
        return -1;
    }

    // check if req_buffer have been allocated correcly
    if (req_buffer == NULL) {
        terminal_print(MSG_ERROR, "Error allocating memory for the buffer", CLIENT, name);
        return -1;
    }

    // serialize the packet to be sent
    if(serialize_request(req_packet, req_buffer) == -1){
        terminal_print(MSG_ERROR, "Error serializing the packet", CLIENT, name);
        return -1;
    }
    
    // send the serialized packet
    if (write(server_fd, req_buffer, strlen(req_buffer)) == -1) {
        free(req_buffer);
        terminal_print(MSG_ERROR, "Failed to send request to the server", CLIENT, name);
        return -1;
    }

    free(req_buffer);

    return 0; // everithing went well

}


int accept_response(int server_fd, ResponsePacket* resp_packet, char* name){

    int character; 

    char* resp_buffer = malloc(BUFFER_SIZE); 
    
    // check if resp_buffer have been allocated correcly
    if (resp_buffer == NULL) {
        terminal_print(MSG_ERROR, "Error allocating memory for the buffer", CLIENT, name);
        return -1;
    }


    // read what camed trought the socket
    character = read(server_fd, resp_buffer, BUFFER_SIZE - 1);
    if (character <= 0) {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Error reading from socket", CLIENT, name);
        return -1;
    }

     // Deserialize the packet using the dedicated function
    if (deserialize_response(resp_buffer, resp_packet) != 0) {
        free(resp_buffer);
        terminal_print(MSG_ERROR, "Error deserializing the packet", CLIENT, name);
        return -1;
    }

    free(resp_buffer);

    return 0;   // everithing went well

}

int connect_to_server(int *server_fd, struct sockaddr_in *serv_addr){
    int connection_count = 0;
    int result;

    // socket creation
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // catch a possible error creating the socket
    if (*server_fd < 0)
    {
        return -1;
    }

    // connection loop ( it will try to connect to the server for 15 sec )
    do
    {
        result = connect(*server_fd, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in));

        if (result == -1)
        { // what a sec and try again
            connection_count++;
            sleep(1);
        }
    } while (connection_count < 15 && result == -1);

    // in case the connection goes timeup
    if (connection_count == 15)
    {
        return -1;
    }

    return 0; // everithing went well
}