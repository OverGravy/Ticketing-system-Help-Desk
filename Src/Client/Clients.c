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
    }

    if (client_stop(server_fd) == -1)
    {
        terminal_print(MSG_ERROR, "Client process wasn't able to close propertly", CLIENT, name);
        return -1;
    }

    terminal_print(MSG_SUCCESS, "Client process completed successfully", CLIENT, name);

    return 0; // Return 0 to indicate success
}

int agent_client_loop(int id, char *name, int client_fd)
{ // the id is fondamental to identify a specific client

    char buffer[256];
    int key;

    RequestPacket req; // package that will be used to send request to the server
    ResponsePacket resp;

    // popolate the request ticket for Sing in
    req.type = REQ_SIGNIN;
    req.role = ROLE_AGENT;
    req.data.signin.agent_id = id;

    // Serialize the packet into a buffer
    if (serialize_request(&req, buffer) != 0)
    {
        terminal_print(MSG_ERROR, "Error serializing packet", AGENT, name);
        return -1;
    }

    // send the message to the server
    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent < 0)
    {
        terminal_print(MSG_ERROR, "Error sending message", AGENT, name);
        return -1;
    }

    // recive the message
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0)
    {
        terminal_print(MSG_ERROR, "Error receiving response", AGENT, name);
        return -1;
    }
    buffer[bytes_received] = '\0'; // put the string terminator

    // deserialize the response
    if (deserialize_response(buffer, &resp) != 0)
    {
        terminal_print(MSG_ERROR, "Error deserializing response", AGENT, name);
        return -1;
    }

    // verify the status
    if (resp.type != RESP_SING_IN_OK || resp.status_code != 0)
    {
        terminal_print(MSG_ERROR, "Agent log-in failed", AGENT, name);
        return -1;
    }

    // extract the key from the message
    key = atoi(resp.message);

    terminal_print(MSG_SUCCESS, "Agent logged in successfully", AGENT, "Agent");

    return 0;
}

int client_stop(int client_fd)
{
    if (close(client_fd) < 0)
    {
        return -1;
    }
    return 0; // Return 0 to indicate successful closure
}
