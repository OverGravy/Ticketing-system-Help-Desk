#include "../../Libs/Client/Client.h"

int client_loop(int id, int port, in_addr_t server_ip_addr)
{
    // client name to identify it in the console
    char name[256];
    snprintf(name, sizeof(name), "Client %d", id);

    // operation result
    int op_result;

    bool cloose_program = false;

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

    // struct for graphical use
    Graphical_struct client_graphics = {0};
    strcpy(client_graphics.widnow_title, name);

    // init the clientg window
    if (init_client_window(&client_graphics, name) == -1)
    {
        terminal_print(MSG_ERROR, "Something went wrong while inizialing the client window", CLIENT, name);
        close(server_fd);
        return -1;
    }

    while (!cloose_program)
    {

        // CLIENT REQUEST OPERATION STUB

        // get the request type
        op_result = req_type_graphics(&req_pack, &client_graphics);

        // check if everithing went right and catch any possible error
        if (op_result == 1)
        {
            terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
            cloose_program = true;
            continue;
        }

        // call the right graphics and the send the requestS
        switch (req_pack.type)
        {
        case REQ_CREATE_TICKET: // case the user selected to send a new ticket to the server

            // call the graphical function
            op_result = ticket_graphics(&req_pack, &client_graphics);

            req_pack.data.new_ticket.client_id = id; // set the client id as the sender id

            // check for any error
            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                cloose_program = true;
                continue;
            }

            // check if everithing have been compilated correctly
            if (!strcmp(req_pack.data.new_ticket.title, STR_UNUSED) && !strcmp(req_pack.data.new_ticket.description, STR_UNUSED) && req_pack.data.new_ticket.priority == INT_UNUSED)
            {

                op_result = error_graphics("Title and description can not be empty", &client_graphics);

                if (op_result == 1)
                {
                    terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                    cloose_program = true;
                    continue;
                }

                continue; // skip the rest and retun to the decision menu
            }

            break;

        case REQ_QUERY: // case the user decide to look without registration the status of a specific cazzo di tciket

            op_result = filters_graphics(&req_pack, false, &client_graphics);

            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                cloose_program = true;
                continue;
            }

            // check if any parameter have been set as filter
            if (req_pack.data.Client_query.client_id == INT_UNUSED &&
                req_pack.data.Client_query.ticket_id == INT_UNUSED &&
                !strcmp(req_pack.data.Client_query.title, STR_UNUSED) &&
                !strcmp(req_pack.data.Client_query.description, STR_UNUSED))
            {
                terminal_print(MSG_ERROR, "No filter applied to the query", CLIENT, name);
                op_result = error_graphics("At least one query filter must be applied", &client_graphics);

                // case user close window there
                if (op_result == 1)
                {
                    terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                    cloose_program = true;
                    continue;
                }

                // in other case no request send
                continue;
            }

            break;

        case REQ_QUERY_AND_MOD: // case the user decide to look for a ticket and modify it with the right autenticvation

            // get the filters payload using the graphics;
            op_result = filters_graphics(&req_pack, true, &client_graphics);

            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                cloose_program = true;
                continue;
            }

            // check if any parameter have been set as filter
            if (req_pack.data.Agent_query.filters.client_id == INT_UNUSED &&
                req_pack.data.Agent_query.filters.ticket_id == INT_UNUSED &&
                !strcmp(req_pack.data.Agent_query.filters.title, STR_UNUSED) &&
                !strcmp(req_pack.data.Agent_query.filters.description, STR_UNUSED))
            {
                terminal_print(MSG_ERROR, "No filter applied to the query", CLIENT, name);
                op_result = error_graphics("At least one query filter must be applied", &client_graphics);

                // case user close window there
                if (op_result == 1)
                {
                    terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                    cloose_program = true;
                    continue;
                }

                // in other case no request send
                continue;
            }

            // get the modification payload using the graphics
            op_result = modification_graphics(&req_pack, &client_graphics);

            // catch any potencial error
            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                cloose_program = true;
                continue;
            }

            // check if the password have been insert
            if (req_pack.data.Agent_query.pwd == INT_UNUSED)
            {
                terminal_print(MSG_ERROR, "To request a ticket modification password field must be filled", CLIENT, name);
                op_result = error_graphics("Password field must be filled", &client_graphics);

                // case user close window there
                if (op_result == 1)
                {
                    terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                    cloose_program = true;
                    continue;
                }

                // in other case no request need to be send send
                continue;
            }

            if (req_pack.data.Agent_query.mod.new_client_id == INT_UNUSED &&
                req_pack.data.Agent_query.mod.new_priority == INT_UNUSED &&
                req_pack.data.Agent_query.mod.new_status == (TicketStatus)INT_UNUSED)
            {
                terminal_print(MSG_ERROR, "No modification applied to the query", CLIENT, name);
                op_result = error_graphics("At least modifier filter must be applied", &client_graphics);

                // in case the user closes the window
                if (op_result == 1)
                {
                    terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                    cloose_program = true;
                    continue;
                }

                // in other case no request send
                continue;
            }

            break;

        default:
            break;
        }

        // CLIENT SOCKET OPERATION STUB

        // try to connecto to the server
        if (connect_to_server(&server_fd, &server_addr) == -1)
        {
            terminal_print(MSG_ERROR, "Server conncetion went timeup", CLIENT, name);
            op_result = error_graphics("Unable to connect to the server", &client_graphics);

            // case user close window there
            if (op_result == 1)
            {
                terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
                cloose_program = true;
                continue;
            }

            // in other case no request send
            continue;
        }

        // send the request
        if (send_request(server_fd, &req_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while sending the request", CLIENT, name);
            continue;
        }
        else
        {
            terminal_print(MSG_SUCCESS, "Request sent succesfully to the server", CLIENT, name);
        }

        // wait for a positive response from server
        if (accept_response(server_fd, &resp_pack, name) == -1)
        {
            terminal_print(MSG_ERROR, "Something went wrong while waithing for a response", CLIENT, name);
            continue;
        }

        // CLIENT RESPONSE OPERATION STUB

        // handle the response
        op_result = response_graphics(&resp_pack, name, &client_graphics);

        // catch any error
        if (op_result == 1)
        {
            terminal_print(MSG_INFO, "User terminated the program", CLIENT, name);
            cloose_program = true;
            continue;
        }

        close(server_fd);
    }

    // case it wasn't close before
    close(server_fd);

    stop_client_window(&client_graphics);

    return 0; // Return 0 to indicate success
}