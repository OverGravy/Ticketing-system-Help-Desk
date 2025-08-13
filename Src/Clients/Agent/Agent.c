#include "../../../Libs/Clients/Agent/Agent.h"

int agent_client_loop(int id, int port, in_addr_t server_ip_addr)
{

    int key; // password for the request
    int op_result;

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

    // intestazione fissa request
    req_pack.sender_id = id;
    req_pack.role = ROLE_AGENT;

    // popolate the request ticket for Sing in
    req_pack.type = REQ_SIGNIN;
    req_pack.data.signin.agent_id = id;

    // connect to th server to require sing-in
    if (connect_to_server(&server_fd, &server_addr) == -1)
    {
        terminal_print(MSG_ERROR, "Server conncetion went timeup", AGENT, name);
        return -1;
    }

    // send request for sing-in
    if (send_request(server_fd, &req_pack, name) == -1)
    {
        terminal_print(MSG_ERROR, "Somenthing went wrong while tryng to log in", AGENT, name);
        return -1;
    }

    // get response from the server
    if (accept_response(server_fd, &resp_pack, name) == -1)
    {
        terminal_print(MSG_ERROR, "Some error occured while reciving a response to login from the server", AGENT, name);
        return -1;
    }

    // verify the status
    if (resp_pack.type != RESP_SING_IN_OK)
    {
        terminal_print(MSG_ERROR, "Agent sing-in failed", AGENT, name);
        return -1;
    }
    else
    {
        terminal_print(MSG_SUCCESS, "Agent singged in in successfully", AGENT, name);
    }

    // extract the key from the message
    key = atoi(resp_pack.message);

    if (init_agent_window(id) == -1)
    {
        terminal_print(MSG_ERROR, "Something went wrong while inizialing the agent window", AGENT, name);
        close(server_fd);
        return -1;
    }

    while (1) // main agent_loop
    {

        sleep(2);

        // it is fixed
        req_pack.type = REQ_QUERY_AND_MOD;
        req_pack.data.Agent_query.pwd = key;

        // get the filters payload using the graphics;
        op_result = filters_graphics(&req_pack);

        // catch any potencial error
        if (op_result == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while compiling the filter form", AGENT, name);
            close(server_fd);
            return -1;
        }
        else if (op_result == 1)
        {
            terminal_print(MSG_INFO, "User terminated the program", AGENT, name);
            close(server_fd);
            return 0;
        }

        // check if any parameter have been set as filter
        if (req_pack.data.Agent_query.filters.client_id == INT_UNUSED &&
            req_pack.data.Agent_query.filters.ticket_id == INT_UNUSED &&
            !strcmp(req_pack.data.Agent_query.filters.title, STR_UNUSED) &&
            !strcmp(req_pack.data.Agent_query.filters.description, STR_UNUSED))
        {
            terminal_print(MSG_ERROR, "No filter applied to the query", AGENT, name);
            op_result = no_filter_graphics(name);
            // case user close window there
            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", AGENT, name);
                close(server_fd);
                return 0;
            }

            // in other case no request send
            continue;
        }

        // get the modification payload using the graphics
        op_result = modification_graphics(&req_pack);

        // catch any potencial error
        if (op_result == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while compiling the filter form", AGENT, name);
            close(server_fd);
            return -1;
        }
        else if (op_result == 1)
        {
            terminal_print(MSG_INFO, "User terminated the program", AGENT, name);
            close(server_fd);
            return 0;
        }

        if (req_pack.data.Agent_query.mod.new_client_id == INT_UNUSED &&
            req_pack.data.Agent_query.mod.new_priority == INT_UNUSED &&
            req_pack.data.Agent_query.mod.new_status == INT_UNUSED)
        {
            terminal_print(MSG_ERROR, "No modification applied to the query", AGENT, name);
            op_result = no_mod_graphics(name);

            // in case the user closes the window
            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", AGENT, name);
                close(server_fd);
                return 0;
            }

            // in other case no request send 
            continue;
        }

        // connect to the server
        if (connect_to_server(&server_fd, &server_addr) == -1)
        {
            terminal_print(MSG_ERROR, "Server conncetion went timeup", AGENT, name);
            return -1;
        }

        // send the request
        if (send_request(server_fd, &req_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "A problem occured while sending a request to the server", AGENT, name);
            close(server_fd);
            continue;
        }

        // wait the response
        if (accept_response(server_fd, &resp_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "An error occured while reciving a response from the server", AGENT, name);
            continue;
        }

        // show the response using graphics
        op_result = response_graphics(&resp_pack, name);

        // check if user closes the window
        if (op_result == 1)
        {
            terminal_print(MSG_INFO, "User terminated the program", AGENT, name);
            close(server_fd);
            return 0;
        }

        close(server_fd);
    }

    return 0;
}
