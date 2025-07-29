#include "../Libs/Std_client.h"

int client_start(int port){
    
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
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

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


int client_loop(int id,int client_fd){
    
    sleep(1);
    
    char buffer[256];
    printf("Client %d: Sending message to the server\n", id);
    strcpy(buffer, "title|description|01/01/2023|1"); 

    // send the message to the server
    int bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        printf("Client: Error sending message\n");
        return -1;
    }

    printf("Client %d: Message sent successfully\n", id);

    return 0; // Return 0 to indicate success
    
}