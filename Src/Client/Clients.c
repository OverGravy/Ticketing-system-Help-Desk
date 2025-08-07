#include "../../Libs/Client/Clients.h"

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

    while (1)
    {
        sleep(2);

        // try to connecto to the server
        if (connect_to_server(&server_fd, &server_addr) == -1)
        {
            terminal_print(MSG_ERROR, "Server conncetion went timeup", CLIENT, name);
            return -1;
        }

        // field that client can set
        req_pack.type = REQ_CREATE_TICKET;
        req_pack.data.new_ticket.client_id = id;
        strcpy(req_pack.data.new_ticket.description, "lol");
        strcpy(req_pack.data.new_ticket.title, "jimmy");
        req_pack.data.new_ticket.priority = 10;

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
        else
        {
            if (resp_pack.type == RESP_TICKET_OK)
            {
                terminal_print(MSG_INFO, "Ticket correctly accepted by the server", CLIENT, name);
            }
            else
            {
                terminal_print(MSG_ERROR, "Something went wrong on server side while accepting the ticket", CLIENT, name);
            }
        }
        

        close(server_fd);
        

        // search for the ticket in the servr
        req_pack.type = REQ_QUERY;
        req_pack.role = CLIENT;
        req_pack.sender_id = id;
        req_pack.data.filters.client_id = id;
        
         // try to connecto to the server
        if (connect_to_server(&server_fd, &server_addr) == -1)
        {
            terminal_print(MSG_ERROR, "Server conncetion went timeup", CLIENT, name);
            return -1;
        }




    }

    // case it wasn't close before
    close(server_fd);

    terminal_print(MSG_SUCCESS, "Client process completed successfully", CLIENT, name);

    return 0; // Return 0 to indicate success
}

int agent_client_loop(int id, int port, in_addr_t server_ip_addr)
{ 

    int key;

    // client name to identify it in the console
    char name[256];
    snprintf(name, sizeof(name), "Agent %d", id);

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

    // popolate the request ticket for Sing in
    req_pack.type = REQ_SIGNIN;
    req_pack.role = ROLE_AGENT;
    req_pack.data.signin.agent_id = id;

    // connect to th server to require sing-in
    if (connect_to_server(&server_fd, &server_addr) == -1)
    {
        terminal_print(MSG_ERROR, "Server conncetion went timeup", AGENT, name);
        return -1;
    }

    // send request for sing-in
    if (send_request(server_fd, &req_pack, name) == -1){
        terminal_print(MSG_ERROR, "Somenthing went wrong while tring to log in", AGENT, name);
        return -1;
    }

    // get response from the server
    if (accept_response(server_fd, &resp_pack, name) == -1){
        terminal_print(MSG_ERROR, "Some error occured while redciving a response to login from th server", AGENT, name);
        return -1;
    }

    // verify the status
    if (resp_pack.type != RESP_SING_IN_OK)
    {
        terminal_print(MSG_ERROR, "Agent log-in failed", AGENT, name);
        return -1;
    }else{
         terminal_print(MSG_SUCCESS, "Agent logged in successfully", AGENT, name);
    }

    // extract the key from the message
    key = atoi(resp_pack.message);

   

    return 0;
}
