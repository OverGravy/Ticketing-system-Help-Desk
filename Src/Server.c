#include "../Libs/Server.h"
#include "../Libs/Tickets_list.h"

int server_start(int port){

    int server_fd;                  // socket file descriptor
    struct sockaddr_in server_addr; // necessessary struct for the binding fase
    
    // 1. Socket creation
    server_fd = socket(AF_INET,SOCK_STREAM, DEFAULT_PROTOCOL);
    if(server_fd == -1){
        return -1;
    }

    // 2. Setup Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);
    
    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0) {
        return -1;
    }

    signal(SIGCHLD, SIG_IGN); // ignore childs death 

   return server_fd;
}


int server_loop(int server_fd){

    int client_fd;                                   // client file descriptor
    struct sockaddr_in client_addr;                  // client info compiled by accept
    socklen_t client_len = sizeof(client_addr);

    ssize_t received;

    // create Tickets list pointer
    struct TicketNode* tickets_list = NULL;

    while(1){ // TROVA UN CONDIZIONE SERIA

      client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Server: Error accepting the message");
            continue;  // skip without terminte the server
        }

        // show client information
        printf("Server: connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // create a child process to handle the connection
        if(fork() == 0){

            if(Ticket_handler(client_fd, tickets_list, BUFFER_SIZE) < 0){
                printf("Server: ticket memorized");
            }else{
                printf("Server: an error occurd while memorizing a ticket");
            }    

            // close the socket and terminate the child that handled the connection
            close(client_fd);         // close socket with that specific client
            exit (/* EXIT_SUCCESS */ 0); 

        }else{
            perror("Server: unable to fork and handle the connection");
            close(client_fd);
        }
        return 0;
    }
}