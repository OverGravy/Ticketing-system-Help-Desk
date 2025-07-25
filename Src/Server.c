#include "../Libs/Server.h"

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
    server_addr.sin_addr.s_addr = INADDR_LOOPBACK;
    server_addr.sin_port = htons(port);
    
    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0) {
        return -1;
    }

   return server_fd;
}


int server_loop(int socket){

    // create Tickets list pointer
    struct TicketNode* tickets_list = NULL;

    while(1){ // TROVA UN CONDIZIONE SERIA

        
        
    }
}