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
    sqlite3 *db;                                // database needed to store tickets
    struct sockaddr_in client_addr;             // client info compiled by accept
    socklen_t client_len = sizeof(client_addr); // size of the client address structure

    RequestPacket req_packet;
    ResponsePacket resp_packet;

    signal(SIGCHLD, SIG_IGN); // ignore childs death

    // init the database and catch any possible error while doing it
    if (init_db(&db, "../Db/Tickets.db") != SQLITE_OK)
    {
        terminal_print(MSG_ERROR, "Something went wrong while trying to inizialize the database", SERVER, "Server");
        return -1;
    }
    else
    {
        terminal_print(MSG_INFO, "Database correctly inizialized ...", SERVER, "Server");
    }

    while (1)
    {

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
            case REQ_CREATE_TICKET:
               
                request_add_ticket(db, &req_packet, &resp_packet);
                    
                break;

            case REQ_SIGNIN:

                request_sing_in(db, &req_packet, &resp_packet);
              
                break;

            case REQ_QUERY:

                request_client_query(db, &req_packet, &resp_packet);
                
                break;
            case REQ_QUERY_AND_MOD:

                request_agent_query(db, &req_packet, &resp_packet);

                break;
            default:
                terminal_print(MSG_ERROR, "Unknown request type", SERVER, "Server");
                resp_packet.type = RESP_ERROR;
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

    // close teh database
    if (close_database(db) != SQLITE_OK)
    {
        terminal_print(MSG_ERROR, "Something went wrong while closing the tickets database", SERVER, "Server");
    }

    close(server_fd);

    return 0;
}

