#include "../../Libs/Server/Request_handler.h"

void request_add_ticket(sqlite3 *db, int ticket_count, RequestPacket *request, ResponsePacket *response)
{
    int op_result;

    // set incremental ticket id, i've decided this is a thing server have to do not the db
    request->data.new_ticket.id = ticket_count;

    // set default status to the request
    request->data.new_ticket.status = STATUS_OPEN;

    // set actual date to the ticket
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(request->data.new_ticket.date, sizeof(request->data.new_ticket.date), "%d/%m/%Y", t);

    // set priotity = 0 if unsetted by the client
    if (request->data.new_ticket.priority == INT_UNUSED)
    {
        request->data.new_ticket.priority = 0;
    }

    // cal the right database function
    op_result = add_ticket(db, &request->data.new_ticket);
    if (op_result != SQLITE_OK)
    {
        response->type = RESP_ERROR;
        strcpy(response->message, "Something occurds while adding the ticket on the database");
        terminal_print(MSG_ERROR, "Error inserting ticket inside the database", SERVER, "Server");
    }
    else
    {
        response->type = RESP_TICKET_OK;
        strcpy(response->message, "Everithing went fine");
        terminal_print(MSG_SUCCESS, "Ticket added successfully", SERVER, "Server");
    }
}

void request_sing_in(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    char msg[256];
    int op_result;

    // generate a key for the user
    srand(time(NULL));
    int pwd = request->sender_id * 12345 + (rand() % 2000 + 1); // even if the id is 0 you have a number that operate as a key

    // add the agent to the db
    op_result = add_agent(db, request->sender_id, pwd);

    // check the result of the operation
    if (op_result == -1)
    {
        terminal_print(MSG_ERROR, "Error signing in Client", SERVER, "Server");
        response->type = RESP_ERROR;
        strcpy(response->message, "Something went wrong while singing in the Client");
    }
    else if (op_result == 1)
    {
        terminal_print(MSG_INFO, "Client already signed in", SERVER, "Server");
        response->type = RESP_ERROR;
        strcpy(response->message, " Client already sing in");
    }
    else
    {
        sprintf(msg, "Client %d", request->sender_id);
        strcat(msg, " Sing in succesfully");
        terminal_print(MSG_SUCCESS, msg, SERVER, "Server");

        // prepare the response for the agent
        response->type = RESP_SING_IN_OK;
        snprintf(response->message, sizeof(response->message), "Client passowrd: %d", pwd); // send the key inside the message
    }
}

void request_client_query(sqlite3 *db, RequestPacket *request, ResponsePacket *response, TicketPile *pile)
{
    int op_result = get_ticket(db, &request->data.Client_query, pile);

    // clear response message buffer
    response->message[0] = '\0';

    size_t cap = sizeof(response->message);
    size_t used = 0;

    if (op_result > 0)
    {
        response->type = RESP_QUERY_OK;

        // write header
        int n = snprintf(response->message, cap,
                         "Found %d ticket%s\n",
                         op_result, (op_result == 1) ? "" : "s");

        if (n < 0)
            n = 0;
        if ((size_t)n >= cap)
        {
            response->message[cap - 1] = '\0';
            return; // full buffer, cannot write header
        }
        used = (size_t)n;

        // add each ticket to the response message
        Ticket current_ticket;
        for (int i = 0; i < op_result; i++)
        {
            if (pop_ticket(pile, &current_ticket) != 0)
            {
                break; // pop error
            }
            
            // Scrive solo se c'è spazio sufficiente
            n = snprintf(response->message + used, cap - used,
                         "Ticket ID: %d  Title: %.100s  Description: %.200s Date: %.20s  Priority: %d  Status: %d  Client ID: %d\n",
                         current_ticket.id,
                         current_ticket.title,
                         current_ticket.description,
                         current_ticket.date,
                         current_ticket.priority,
                         current_ticket.status,
                         current_ticket.client_id);

            if (n < 0)
                break;

            // Se la scrittura supera lo spazio residuo, tronca correttamente
            if ((size_t)n >= cap - used)
            {
                response->message[cap - 1] = '\0';
                break;
            }

            used += (size_t)n;
        }
    }
    else if (op_result == 0)
    {
        response->type = RESP_ERROR;
        terminal_print(MSG_INFO, "Client query found no tickets matching the query", SERVER, "Server");
        strncpy(response->message, "No tickets found matching the query", cap - 1);
        response->message[cap - 1] = '\0';
    }
    else
    {
        response->type = RESP_ERROR;
        terminal_print(MSG_ERROR, "Something went wrong while trying to get tickets from the database", SERVER, "Server");
        strncpy(response->message, "Something went wrong on server side", cap - 1);
        response->message[cap - 1] = '\0';
    }
}

void request_agent_query(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    int op_result;

    int table_pwd;

    op_result = get_agent_password(db, request->sender_id, &table_pwd);
    if (op_result == 0)
    {
        // case the agent is logged in corrrectly
        terminal_print(MSG_INFO, "Client log-in succesfully", SERVER, "Server");
        op_result = get_ticket_and_mod(db, &request->data.Agent_query);
        if (op_result == 0)
        {
            terminal_print(MSG_SUCCESS, "Client modified succesfully one ticket", SERVER, "Server");
            response->type = RESP_QUERY_MOD_OK;
            strcpy(response->message, "Everithing went fine");
        }
        else if (op_result == 2)
        {
            terminal_print(MSG_INFO, "Client query found no tickets matching the query", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, "No tickets found matching the query");
        }
        else if (op_result == 1)
        {
            terminal_print(MSG_INFO, "Client query found more than one ticket matching the query", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, "More than one ticket found matching the query, impossible to modify\nTry to be more specific with filters");
        }
        else
        {
            terminal_print(MSG_ERROR, "Something went wrong while trying to get/modify ticket from the database", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, "Something went wrong on server side");
        }
    }

    // case agent is not sing in
    else if (op_result == 1)
    {
        terminal_print(MSG_INFO, "Client tried to lo-in without sing-in", SERVER, "Server");
        response->type = RESP_AUTH_REQUIRED;
        strcpy(response->message, "Client need to sing-in first");
    }
    else
    {
        response->type = RESP_ERROR;
        strcpy(response->message, "Something went wrong on server side");
        terminal_print(MSG_ERROR, "Something went wrong while trying to get the agent password from the database", SERVER, "Server");
    }
}