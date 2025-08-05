#include "../../Libs/Client/Clients.h"

int client_start(int port, in_addr_t server_ip_addr){
    
    int client_fd, server_len;
    struct sockaddr_in server_addr;

    struct sockaddr* server_sock_addr_ptr;
    server_sock_addr_ptr=(struct sockaddr*)&server_addr;
    server_len = sizeof (server_addr);


    // 1. Socket creation
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    // catch a possible error creating the socket
    if (client_fd < 0) {
        return -1;
    }

    // 2. Setup Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (port);
    server_addr.sin_addr.s_addr = htonl(server_ip_addr); 

    int connection_count = 0;
    int result;

    // connection loop ( it will try to connect to the server for 15 sec )
    do{
        result=connect(client_fd, server_sock_addr_ptr, server_len);
        
        if (result == -1) {   // what a sec and try again
            connection_count ++;
            sleep (1);
        }
    }while(connection_count < 15 && result == -1);

    // in case the connection goes timeup
    if(connection_count == 15){
        return -1;
    }

    return client_fd;
}


int client_loop(int id,int client_fd){   // the id is fondamental to identify a specific client
    
    sleep(1);
    
    char buffer[256];

    /*
    // Create a dummy ticket to send
    struct Ticket ticket = {
        .title = "Test Ticket", // Ticket title
        .description = "This is a test ticket", // Ticket description
        .creation_date = malloc(sizeof(struct Date)), // Allocate memory for creation date
        .priority = 1, // Ticket priority
        .status = 0, // Ticket status
        .agent_id = id, // Agent ID associated with the ticket
    };

    // Check if memory allocation for creation_date was successful
    if (ticket.creation_date == NULL) {
        terminal_print(MSG_ERROR, "Memory allocation failed for ticket creation date", CLIENT, "Client");
        free(ticket.creation_date); // Free memory if allocation failed
        return -1; // Return error
    }
    // Set creation date fields
    ticket.creation_date->day = 1;
    ticket.creation_date->month = 1;
    ticket.creation_date->year = 2023;

    // Serialize the ticket into a packet
    struct Packet packet;
    packet.type = 2; // Assuming type 2 is for ticket creation
    packet.contenct = ticket; // Assign ticket to packet content

    // Serialize the packet into a buffer
    if (serialize_packet(&packet, buffer, sizeof(buffer)) != 0) {
        terminal_print(MSG_ERROR, "Error serializing packet", CLIENT, "Client");
        free(ticket.creation_date); // Free memory if serialization fails
        return -1; // Return error
    }
    // Free the allocated memory for the ticket creation date
    free(ticket.creation_date);

    // Send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        terminal_print(MSG_ERROR, "Error sending message", CLIENT, "Client");
        return -1; // Return error if sending fails
    }

    // Print success message
    terminal_print(MSG_SUCCESS, "Ticket sent successfully", CLIENT, "Client");
    */
    while(1) { // Loop until the window is closed
    
    }

    // send a message to the server to indicate that the client is closing
    /*
    struct Packet packet;
    packet.type = 5;
    packet.contenct.agent_id = id; // Set the agent ID in the packet content
    
    // Serialize the packet into a buffer
    if (serialize_packet(&packet, buffer, sizeof(buffer)) != 0) {
        terminal_print(MSG_ERROR, "Error serializing packet", CLIENT, "Client");
        return -1; // Return error if serialization fails
    }
    
    // Send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        terminal_print(MSG_ERROR, "Error sending message", CLIENT, "Client");
        return -1; // Return error if sending fails
    }
    
    // Print success message
    terminal_print(MSG_SUCCESS, "Client closing message sent successfully", CLIENT, "Client");
    */

    // Close the client socket
    close(client_fd);
    
    terminal_print(MSG_SUCCESS, "Client process completed successfully", CLIENT, "Client");

    return 0; // Return 0 to indicate success
    
}


int agent_client_loop(int id,int client_fd){   // the id is fondamental to identify a specific client
    
    char buffer[256];
    int key;
    
    RequestPacket req; // package that will be used to send request to the server 
    ResponsePacket resp;

    // popolate the request ticket for Sing in
    req.type = REQ_SIGNIN;
    req.role = ROLE_AGENT;
    req.data.signin.agent_id = id;

    // Serialize the packet into a buffer
    if (serialize_request(&req, buffer) != 0) {
        terminal_print(MSG_ERROR, "Error serializing packet", AGENT, "Agent");
        return -1;
    }

    // send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        terminal_print(MSG_ERROR, "Error sending message", AGENT, "Agent");
        return -1;
    }

    // recive the message
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0) {
        terminal_print(MSG_ERROR, "Error receiving response", AGENT, "Agent");
        return -1;
    }
    buffer[bytes_received] = '\0'; // put the string terminator

    // deserialize the response
    if (deserialize_response(buffer, &resp) != 0) {
        terminal_print(MSG_ERROR, "Error deserializing response", AGENT, "Agent");
        return -1;
    }

    // verify the status
    if (resp.type != RESP_OK || resp.status_code != 0) {
        terminal_print(MSG_ERROR, "Agent log-in failed", AGENT, "Agent");
        return -1;
    }

    // extract the key from the message
    key = atoi(resp.message);  

    terminal_print(MSG_SUCCESS, "Agent logged in successfully", AGENT, "Agent");

    
    return 0;
}

int client_stop(int client_fd) {
    if (close(client_fd) < 0) {
        terminal_print(MSG_ERROR, "Error closing client socket", CLIENT, "Client");
        return -1;
    }
    return 0; // Return 0 to indicate successful closure
}
