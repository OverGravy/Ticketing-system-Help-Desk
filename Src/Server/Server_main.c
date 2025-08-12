#include "../../Libs/Server/Server.h"
#include "../../Libs/Terminal_com.h"

int main(int argc, char *argv[]) {
    int server_fd;

    terminal_print(MSG_INFO, "Starting server process...", SERVER, "Server");
    server_fd = server_start(DEFAULT_PORT);

    // check if the file desriptor havs been created correcly
    if(server_fd < 0){
        terminal_print(MSG_ERROR, "Failed to start the server", SERVER, "Server");
        return -1; // something went wrong
    }

    // execute the sever loop catching any error
    if(server_loop(server_fd) < 0) return -1; // something went wrong in server loop

    // finaml message
    terminal_print(MSG_INFO, "Server stopped successfully", SERVER, "Server");
    
    return 0;

}

