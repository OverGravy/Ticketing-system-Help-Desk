#include "../../../Libs/Clients/Client/Client.h"

int client_loop(int id, int port, in_addr_t server_ip_addr)
{
    // client name to identify it in the console
    char name[256];
    snprintf(name, sizeof(name), "Client %d", id);

    // server info
    int server_fd;
    struct sockaddr_in server_addr;

    // address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(server_ip_addr);

    // packets needed for the comunication
    RequestPacket req_pack;
    ResponsePacket resp_pack;

    // intestazione fissa
    req_pack.sender_id = id;
    req_pack.role = ROLE_CLIENT;

    // init the clientg window
    if (init_agent_window(id) == -1)
    {
        terminal_print(MSG_ERROR, "Something went wrong while inizialing the client window", CLIENT, name);
        close(server_fd);
        return -1;
    }

    while (1)
    {
        // get the request type

        // considering the request type call the right graphical form

        // try to connecto to the server
        if (connect_to_server(&server_fd, &server_addr) == -1)
        {
            terminal_print(MSG_ERROR, "Server conncetion went timeup", CLIENT, name);
            return -1;
        }

        // send the request
        if (send_request(server_fd, &req_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while sending the request", CLIENT, name);
        }
        else
        {
            terminal_print(MSG_SUCCESS, "Request sent succesfully to the server", CLIENT, name);
        }

        // wait for a positive response from server
        if (accept_response(server_fd, &resp_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while waithing for a response", CLIENT, name);
        }

        // handle the respond based on the request type

        close(server_fd);
    }

    // case it wasn't close before
    close(server_fd);

    terminal_print(MSG_SUCCESS, "Client process completed successfully", CLIENT, name);

    return 0; // Return 0 to indicate success
}