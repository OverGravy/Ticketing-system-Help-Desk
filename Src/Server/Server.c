#include "../../Libs/Server/Server.h"

int server_start(int port){

    int server_fd;                  // socket file descriptor
    struct sockaddr_in server_addr; // necessessary struct for the binding fase
    
    // 1. Socket creation
    server_fd = socket(AF_INET,SOCK_STREAM, DEFAULT_PROTOCOL);
    if(server_fd == -1){
        terminal_print(MSG_ERROR, "Error creating socket", SERVER, "Server");
        return -1;
    }

    // 2. Setup Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);
    
    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        terminal_print(MSG_ERROR, "Error binding socket", SERVER, "Server");
        return -1;
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0) {
        terminal_print(MSG_ERROR, "Error listening on socket", SERVER, "Server");
        return -1;
    }

   return server_fd;
}


int server_loop(int server_fd){

    int client_fd;                                   // client file descriptor
    struct sockaddr_in client_addr;                  // client info compiled by accept
    socklen_t client_len = sizeof(client_addr);      // size of the client address structure

    signal(SIGCHLD, SIG_IGN); // ignore childs death 

    // create Tickets list pointer
    struct TicketNode* tickets_list = NULL;
    // create Agents list pointer
    struct AgentNode *agent_list = NULL;

    while(1){ // TROVA UN CONDIZIONE SERIA

      client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            terminal_print(MSG_ERROR, "Error accepting client connection", SERVER, "Server");
            continue;  // skip without terminte the server
        }

        // show client information
        char *infos;
        infos = inet_ntoa(client_addr.sin_addr);
        strcat(infos, " connected to the server");
        terminal_print(MSG_INFO, infos, SERVER, "Server");

        // create a child process to handle the connection
        int request_handler_pid = fork();
        if(request_handler_pid == 0){

            // check if the Ticket_handler function return 0, if so the ticket is memorized
            if(request_handler(client_fd, tickets_list, BUFFER_SIZE, agent_list) != 0){
                terminal_print(MSG_ERROR, "Error handling request", SERVER, "Server");
            }    

            // close the socket and terminate the child that handled the connection
            close(client_fd);         // close socket with that specific client
            exit (/* EXIT_SUCCESS */ 0); 

        }else if(request_handler_pid<0) {
            terminal_print(MSG_ERROR, "Error forking process for request handler", SERVER, "Server");
            close(client_fd);
        }
        //exit(EXIT_SUCCESS); // exit the parent process to avoid creating a zombie process
    }
    return 0;
}



int server_stop(int server_fd) {
    if (close(server_fd) < 0) {
        terminal_print(MSG_ERROR, "Error closing server socket", SERVER, "Server");
        return -1;
    }
    return 0;
}