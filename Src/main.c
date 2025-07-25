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
    int num_clients = rand() % 5 + 1; // Random number between 0, 5

    // main maintains an array of all clients pids
    int Std_client_pids[num_clients];
    printf("Main: About to launch %d standard clients ...", num_clients);
    for(int i=0; i<num_clients; i++){
        Std_client_pids[i] = fork();
        if(Std_client_pids[i] == 0){
            printf("Client %d: starting client process...", i);
            if(client_loop(i) == -1){
                printf("Client %d: failed to start", i);
            }
        } else if(Std_client_pids[i] < 0) {
            printf("Main: Failed to fork standard client %d\n", i);
        }
    }

    // DOVRO SPAWNARE ANCHE GLI AGENTI

    // QUA DOVRO ASPETTARE LA MORTE DEI FIGLIO O FARE QUALCOSA IN GENERALE
    
    return 0; // Return 0 to indicate successful execution
}