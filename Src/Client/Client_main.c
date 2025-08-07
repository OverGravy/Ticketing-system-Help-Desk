#include "../../Libs/Client/Clients.h"

// The client needs is id as argument on argv
int main(int argc, char *argv[]) {

    int id; // client id expected to be pass on argv in case it will be 0
    char name[256];

    if(argc == 2){
        id = atoi(argv[1]); // retrieve the id
        snprintf(name, sizeof(name), "Client %s", argv[1]);
    }else{
        id = 0;
        snprintf(name, sizeof(name), "Client 0");
    }

    // starting client operation 
    terminal_print(MSG_INFO, "Starting standard client process...", CLIENT, name); 
    
    // execute the client loop catching any possible error
    if(client_loop(id, DEFAULT_PORT, INADDR_LOOPBACK) < 0) return -1;

    // final message
    terminal_print(MSG_INFO, "Client stopped successfully", CLIENT, name);

    return 0;

}