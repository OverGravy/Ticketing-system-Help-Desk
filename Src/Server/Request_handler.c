#include "../../Libs/Server/Request_handler.h"

int request_add_ticket(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    int op_result;

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

    return 0;
}

int request_sing_in(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    int op_result;
    if (request->role == ROLE_AGENT)
    {
        op_result = Singing_in(db, request->data.signin.agent_id);
        if (op_result == -1)
        {
            terminal_print(MSG_ERROR, "Error signing in agent", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, "Something went wrong while singing in the agent");
        }
        else if (op_result == 0)
        {
            terminal_print(MSG_INFO, "Agent already signed in", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, " Agent already sing in");
        }
        else
        {
            terminal_print(MSG_SUCCESS, "Agent signed in successfully", SERVER, "Server");

            // prepare the response for the agent
            response->type = RESP_SING_IN_OK;
            snprintf(response->message, sizeof(response->message), "%d", op_result); // send the key inside the message
        }
    }
    else
    {
        terminal_print(MSG_INFO, "An anauthorized client tried to log in", SERVER, "Server");
        response->type = RESP_ERROR;
        strcpy(response->message, "An user cant log as an agent");
    }

    return 0;
}

int request_client_query(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    int op_result;

    Ticket query;

    // result of a find
    op_result = get_ticket(db, &request->data.Client_query, &query);

    // if the function finds it inside the list
    if (op_result == SQLITE_OK)
    {
        terminal_print(MSG_SUCCESS, "Ticket succcesfuylly found in database", SERVER, "Server");
        response->type = RESP_QUERY_OK;
        snprintf(response->message, sizeof(response->message), "%d", query.status);
    }
    else if (op_result == SQLITE_DONE)
    {
        terminal_print(MSG_ERROR, "Ticket wasn't found inside the database", SERVER, "Server");
        response->type = RESP_ERROR;
        strcpy(response->message, "Ticket wasn't find in the database");
    }
    else
    {
        terminal_print(MSG_ERROR, "An error occured while querying the database", SERVER, "Server");
        response->type = RESP_ERROR;
        strcpy(response->message, "Somenthing wrong happen while searching the ticket in the database");
    }

    return 0;
}

int request_agent_query(sqlite3 *db, RequestPacket *request, ResponsePacket *response)
{
    int op_result;
    if (request->role == ROLE_AGENT)
    {
        op_result = Logging_in(db, request->sender_id, request->data.Agent_query.pwd);
        if (op_result == 0)
        {
            // case the agent is logged in corrrectly
            terminal_print(MSG_INFO, "Agent log-in succesfully", SERVER, "Server");
            op_result = get_ticket_and_mod(db, &request->data.Agent_query);
            if (op_result == 0)
            {
                terminal_print(MSG_SUCCESS, "An Agent modified succesfully one ticket", SERVER, "Server");
                response->type = RESP_QUERY_MOD_OK;
                strcpy(response->message, "everithing went fine");
            }
            else if (op_result == SQLITE_NOTFOUND)
            {
                response->type = RESP_ERROR;
                strcpy(response->message, "record no found");
            }
            else
            {
                response->type = RESP_ERROR;
                strcpy(response->message, "something went wrong on server side");
            }
        }

        // case the password is wrong
        else if (op_result == 1)
        {
            terminal_print(MSG_INFO, "An agent try to lo-in with the wrong password", SERVER, "Server");
            printf("password %d\n", request->data.Agent_query.pwd);
            response->type = RESP_AUTH_REQUIRED;
            strcpy(response->message, "wrong password");
        }

        // errior case
        else if (op_result == -2)
        {
            terminal_print(MSG_ERROR, "Somenthing went wrong while trying to acces the database to get an agent password", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, " somenthing went wrong on server side");
        }
    }
    else
    {
        response->type = RESP_AUTH_REQUIRED;
        strcpy(response->message, "No bro sei un client non puoi loggarti");
    }

    return 0;
}