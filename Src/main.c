#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Libs/Server.h"
#include "../Libs/Std_client.h"
#include "../Libs/Agent_client.h" 

int main() {

    srand(time(NULL));
    
    // start the execution of the server 
    int srv_pid = fork();
    int srv_fd;

    if(srv_pid == 0) { 
        printf("Server: Starting server process...\n");
        srv_fd = server_start(DEFAULT_PORT);

        if(srv_fd < 0){
            printf("Server: Server failed to start");
        }else{
            server_loop(srv_fd);
        }
   
    }else if(srv_pid<0){
        printf("Main: Failed to fork to create the server %d\n");
    }

    // randomize the number of client to be launch
    int num_clients =  1;//rand() % 5 + 1; // Random number between 0, 5

    // main maintains an array of all clients pids
    int Std_client_pids[num_clients];
    printf("Main: About to launch %d standard clients ...", num_clients);
    for(int i=0; i<num_clients; i++){
        Std_client_pids[i] = fork();
        if(Std_client_pids[i] == 0){
            printf("Client %d: starting client process...", i);
            int client_fd = client_start(DEFAULT_PORT);

            // check if the client started correctlys
            if(client_fd < 0){
                printf("Client %d: failed to connect to start", i);
                exit(EXIT_FAILURE);
            }

            client_loop(i, client_fd);
            close(client_fd);
            exit(EXIT_SUCCESS);
            
        } else if(Std_client_pids[i] < 0) {
            printf("Main: Failed to fork standard client %d\n", i);
        }
    }
    
    return 0; // Return 0 to indicate successful execution
}