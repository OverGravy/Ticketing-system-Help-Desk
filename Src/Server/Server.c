#include "../../Libs/Server/Server.h"

int server_start(int port)
{

    int server_fd;                  // socket file descriptor
    struct sockaddr_in server_addr; // necessessary struct for the binding fase

    // 1. Socket creation
    server_fd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (server_fd == -1)
    {
        terminal_print(MSG_ERROR, "Error creating socket", SERVER, "Server");
        return -1;
    }

    // 2. Setup Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        terminal_print(MSG_ERROR, "Error binding socket", SERVER, "Server");
        return -1;
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0)
    {
        terminal_print(MSG_ERROR, "Error listening on socket", SERVER, "Server");
        return -1;
    }

    return server_fd;
}

int server_loop(int server_fd)
{

    int client_fd;                              // client file descriptor
    struct sockaddr_in client_addr;             // client info compiled by accept
    socklen_t client_len = sizeof(client_addr); // size of the client address structure

    RequestPacket req_packet;
    ResponsePacket resp_packet;
    int op_result;

    Ticket *query = NULL; // pointer to a specific ticket that i searched

    struct TicketNode *tickets_list = NULL; // Ticket list pointer
    struct AgentNode *agent_list = NULL;    // Agent list pointer

    signal(SIGCHLD, SIG_IGN); // ignore childs death

    while (1)
    { // TROVA UN CONDIZIONE SERIA

        // accept new connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0)
        {
            terminal_print(MSG_ERROR, "Error accepting client connection", SERVER, "Server");
            continue; // skip without terminte the server
        }

        // show client information
        char infos[128];
        snprintf(infos, sizeof(infos), "%s connected to the server", inet_ntoa(client_addr.sin_addr));
        terminal_print(MSG_INFO, infos, SERVER, "Server");

        // create a child process to handle the connection
        int connection_handler_pid = fork();
        if (connection_handler_pid == 0)
        {

            // get the packet
            if (accept_request(client_fd, &req_packet) != 0)
            {
                terminal_print(MSG_ERROR, "Error accepting request", SERVER, "Server");
            }

            // handle the request type
            switch (req_packet.type)
            {
            case REQ_CREATE_TICKET: // request to add a ticket to the server
                op_result = add_ticket(tickets_list, req_packet.data.new_ticket, 20);
                if (op_result == -1)
                {
                    resp_packet.type = RESP_ERROR;
                    strcpy(resp_packet.message, "NO");
                    terminal_print(MSG_ERROR, "Error adding ticket", SERVER, "Server");
                }
                else
                {
                    resp_packet.type = RESP_TICKET_OK;
                    strcpy(resp_packet.message, "OK");
                    terminal_print(MSG_SUCCESS, "Ticket added successfully", SERVER, "Server");
                }

                break;

            case REQ_SIGNIN: // request to sign in
                op_result = Singing_in(req_packet.data.signin.agent_id, agent_list);
                if (op_result == -1)
                {
                    terminal_print(MSG_ERROR, "Error signing in agent", SERVER, "Server");
                }
                else if (op_result == 0)
                {
                    terminal_print(MSG_INFO, "Agent already signed in", SERVER, "Server");
                }
                else
                {
                    terminal_print(MSG_SUCCESS, "Agent signed in successfully", SERVER, "Server");

                    // prepare the response for the agent
                    resp_packet.type = RESP_SING_IN_OK;
                    snprintf(resp_packet.message, sizeof(resp_packet.message), "%d", op_result); // send the key inside the message
                }
                break;

            case REQ_QUERY:

                query = find_ticket(tickets_list, &req_packet.data.filters);

                // if the function finds it inside the list
                if (query != NULL)
                {
                    terminal_print(MSG_SUCCESS, "Ticket succcesfuylly found in the list", SERVER, "Server");
                    resp_packet.type = RESP_QUERY_OK;
                    strcpy(resp_packet.message, "OK");
                }
                else
                {
                    terminal_print(MSG_ERROR, "Ticket was'nt found inside the list", SERVER, "Server");
                    resp_packet.type = RESP_ERROR;
                    strcpy(resp_packet.message, "OK");
                }
                break;
            case REQ_QUERY_AND_MOD:

            break;
            default:
                terminal_print(MSG_ERROR, "Unknown request type", SERVER, "Server");
                resp_packet.type = RESP_ERROR;

                // add a specific message to this situation

                break;
            }

            // send response
            if (send_response(client_fd, &resp_packet) == -1)
            {
                terminal_print(MSG_ERROR, "Unable to send a response", SERVER, "Server");
            }

            // close the socket and terminate the child that handled the connection
            close(client_fd); // close socket with that specific client
            exit(/* EXIT_SUCCESS */ 0);
        }
        else if (connection_handler_pid < 0)
        {
            terminal_print(MSG_ERROR, "Error forking process for request handler", SERVER, "Server");
            close(client_fd);
        }
        else
        {
            close(client_fd);
        }
    }

    // destroy the list and do not leave anithg hanged
    destroy_tickets_list(tickets_list);

    return 0;
}

int server_stop(int server_fd)
{
    if (close(server_fd) < 0)
    {
        terminal_print(MSG_ERROR, "Error closing server socket", SERVER, "Server");
        return -1;
    }
    return 0;
}