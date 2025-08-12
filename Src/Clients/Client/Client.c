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

        // wait 2 seconds
        sleep(2);

        // search for the ticket in the servr
        req_pack.type = REQ_QUERY;
        req_pack.role = ROLE_CLIENT;
        req_pack.sender_id = id;
        req_pack.data.Client_query.ticket_id = INT_UNUSED;
        req_pack.data.Client_query.client_id = id;
        strcpy(req_pack.data.Client_query.title, STR_UNUSED);
        strcpy(req_pack.data.Client_query.description, STR_UNUSED);

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
        else
        {
            if (resp_pack.type == RESP_QUERY_OK)
            {
                terminal_print(MSG_INFO, "Ticket correctly found by inside the server", CLIENT, name);
                char message[256];
                snprintf(message, 256, "Server found the ticket and the status is: %s", resp_pack.message);
                terminal_print(MSG_INFO, message, CLIENT, name);
            }
            else
            {
                terminal_print(MSG_ERROR, "Something went wrong on server side while searching the ticket", CLIENT, name);
            }
        }

        close(server_fd);
    }

    // case it wasn't close before
    close(server_fd);

    terminal_print(MSG_SUCCESS, "Client process completed successfully", CLIENT, name);

    return 0; // Return 0 to indicate success
}