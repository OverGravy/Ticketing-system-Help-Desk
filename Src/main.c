#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#include "../Libs/Server/Server.h"
#include "../Libs/Client/Clients.h"
#include "../Libs/Terminal_com.h"

int main() {

    srand(time(NULL));
    
    // start the execution of the server 
    int srv_pid = fork();
    int srv_fd;

    if(srv_pid == 0) { 
        terminal_print(MSG_INFO, "Starting server process...", SERVER, "Server");
        srv_fd = server_start(DEFAULT_PORT);

        if(srv_fd > 0){
            server_loop(srv_fd);
        }

        close(srv_fd);
        exit(EXIT_SUCCESS);
   
    }else if(srv_pid<0){
        terminal_print(MSG_ERROR, "Failed to fork server process", SERVER, "Server");
        return EXIT_FAILURE;
    }

    sleep(1); // wait for the server to start

    // randomize the number of client to be launch
    int num_clients =  0;//rand() % 5 + 1; // Random number between 0, 5

    // main maintains an array of all clients pids
    int Std_client_pids[num_clients];
    for(int i=0; i<num_clients; i++){
        Std_client_pids[i] = fork();
        if(Std_client_pids[i] == 0){
            terminal_print(MSG_INFO, "Starting standard client process...", CLIENT, "Client");
            int client_fd = client_start(DEFAULT_PORT, INADDR_LOOPBACK);

            // check if the client started correctlys
            if(client_fd < 0){
                terminal_print(MSG_ERROR, "Failed to start client", CLIENT, "Client");
                exit(EXIT_FAILURE);
            }

            client_loop(i, client_fd);
            close(client_fd);
            exit(EXIT_SUCCESS);
            
        } else if(Std_client_pids[i] < 0) {
            printf("Main: Failed to fork standard client %d\n", i);
        }
    }

   // Attendi tutti i client (già presente, ok)
    for (int i = 0; i < num_clients; i++) {
        waitpid(Std_client_pids[i], NULL, 0);
    }
    
    if (fork() == 0) {
        printf("Agent: Starting client process...\n");
        int agent_fd = client_start(DEFAULT_PORT, INADDR_LOOPBACK);

        // check if the client started correctly
        if(agent_fd < 0){
            printf("Agent: failed to start\n");
            exit(EXIT_FAILURE);
        }

        agent_client_loop(1, agent_fd);
        close(agent_fd);
        exit(EXIT_SUCCESS);
    }

    // Ora attendi anche il processo server
    waitpid(srv_pid, NULL, 0);
    
    return 0; // Return 0 to indicate successful execution
}