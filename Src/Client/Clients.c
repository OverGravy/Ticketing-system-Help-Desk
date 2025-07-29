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
    
    // create a dummy ticket to send
    struct Ticket ticket = {
        .title = "Test Ticket",
        .description = "This is a test ticket",
        .creation_date = malloc(sizeof(struct Date)),
        .priority = 1,
        .status = 0,
        .agent_id = id,
    };

    if (ticket.creation_date == NULL) {
        perror("Client: Memory allocation failed for ticket creation date");
        return -1;
    }
    ticket.creation_date->day = 1;
    ticket.creation_date->month = 1;
    ticket.creation_date->year = 2023;

    // Serialize the ticket into a packet
    struct Packet packet;
    packet.type = 2; // Assuming type 1 is for ticket creation
    packet.contenct = ticket;

    // Serialize the packet into a buffer
    if (serialize_packet(&packet, buffer, sizeof(buffer)) != 0) {
        printf("Client %d: Error serializing packet\n", id);
        free(ticket.creation_date);
        return -1;
    }
    // Free the allocated memory for the ticket creation date
    free(ticket.creation_date);

    // send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        printf("Client: Error sending message\n");
        return -1;
    }

    printf("Client %d: Message sent successfully\n", id);

    return 0; // Return 0 to indicate success
    
}


int agent_client_loop(int id,int client_fd){   // the id is fondamental to identify a specific client
    
    char buffer[256];
    int key;
    struct Packet packet;

    sleep(1);
    
    // create a log in ticket to send
    struct Ticket ticket = {
        .title = "-",
        .description = "-",
        .creation_date = malloc(sizeof(struct Date)),
        .priority = - 999,
        .status = -999, 
        .agent_id = id,
    };

    if (ticket.creation_date == NULL) {
        perror("Agent: Memory allocation failed for ticket creation date");
        return -1;
    }
    ticket.creation_date->day = 1;
    ticket.creation_date->month = 1;
    ticket.creation_date->year = 1;

    // Serialize the ticket into a packet
    packet.type = 6; // Assuming type 6 is for agent login
    packet.contenct = ticket;

    // Free the allocated memory for the ticket creation date
    free(ticket.creation_date);

    // Serialize the packet into a buffer
    if (serialize_packet(&packet, buffer, sizeof(buffer)) != 0) {
        printf("Agent %d: Error serializing packet\n", id);
        return -1;
    }

    // send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        printf("Agent: Error sending message\n");
        return -1;
    }

    printf("Agent %d: Log in request sent successfully\n", id);

    read(client_fd, &key, sizeof(key));
    if (key < 0) {
        printf("Agent %d: Error receiving key from server\n", id);
        return -1;
    }
    printf("Agent %d: Received key from server: %d\n", id, key);

    return 0; // Return 0 to indicate success
    
}
