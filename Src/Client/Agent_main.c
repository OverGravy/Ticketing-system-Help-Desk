#include "../../Libs/Client/Clients.h"

// The agent needs is id as argument on argv
int main(int argc, char *argv[]) {

    int id;
    char name[256];         // expected to be sufficent space for "agent" + id 

    if(argc == 2){
        id = atoi(argv[1]); // retrive the id from the argument of the program
    }else{
        id = 0;
    }

    // create name for the terminal comunication
    snprintf(name, sizeof(name), "Agent %s", argv[1]);

    // starting client operation 
    terminal_print(MSG_INFO, "Starting agent client process...", AGENT, name); 

    // execute the client loop catching any possible error
    if(agent_client_loop(id, DEFAULT_PORT, INADDR_LOOPBACK) < 0) return -1;

    // finaml message
    terminal_print(MSG_INFO, "Client stopped successfully", AGENT, name);

    return 0;
}