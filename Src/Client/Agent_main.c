#include "../../Libs/Client/Clients.h"

// The agent needs is id as argument on argv
int main(int argc, char *argv[]) {
/*
    int id, agent_client_fd; // client id expected to be pass on argv in case it will be 0
    char name[256];         // expected to be sufficent space for "agent" + id 

    if(argc == 2){
        id = atoi(argv[1]); // retrive the id from the argument of the program
    }else{
        id = 0;
    }

    // create name for the terminal comunication
    snprintf(name, sizeof(name), "Agent%s", argv[1]);

    // starting client operation 
    terminal_print(MSG_INFO, "Starting agent client process...", AGENT, name); 
    agent_client_fd = client_start(DEFAULT_PORT, INADDR_LOOPBACK);

    // check if the client file descriptor is correct
    if(agent_client_fd < 0){
        terminal_print(MSG_ERROR, "Failed to start the client", AGENT, name);
        return -1; // something went wrong
    }

    // execute the client loop catching any possible error
    if(agent_client_loop(id, name, agent_client_fd) < 0) return -1;

    // execute the client stop catching any possible error 
    if(client_stop(agent_client_fd) < 0) return -1;

    // finaml message
    terminal_print(MSG_INFO, "Client stopped successfully", AGENT, name);

    return 0;

*/
}