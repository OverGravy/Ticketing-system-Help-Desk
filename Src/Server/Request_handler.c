#include "../../Libs/Server/Request_handler.h"

int request_add_ticket(sqlite3* db, int ticket_count, RequestPacket* request, ResponsePacket* response)
{
    int op_result;

    // set incremental ticket id, i've decided this is a thing server have to do not the db
    request->data.new_ticket.id = ticket_count;

    // set default status to the request 
    request->data.new_ticket.status = STATUS_OPEN;

    // set actual date to the ticket 


    //set priotity = 0 if unsetted by the client
    if(request->data.new_ticket.priority == INT_UNUSED){
        request->data.new_ticket.priority = 0;
    }

    // cal the right database function
    op_result = add_ticket(db, &request->data.new_ticket);
    if (op_result != SQLITE_OK)
    {
        response->type = RESP_ERROR;
        strcpy(response->message, "Something occurds while adding the ticket on the database");
        terminal_print(MSG_ERROR, "Error inserting ticket inside the database", SERVER, "Server");
        return -1;
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
    char msg[256];
    int op_result;
   
        op_result = Singing_in(db, request->sender_id);
        if (op_result == -1)
        {
            
            terminal_print(MSG_ERROR, "Error signing in Client", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, "Something went wrong while singing in the Client");
        }
        else if (op_result == 0)
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
            snprintf(response->message, sizeof(response->message), "Client passowrd: %d", op_result); // send the key inside the message
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
        snprintf(response->message, sizeof(response->message), "Status: %d , Priority: %d ", query.status);
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
   
        op_result = Logging_in(db, request->sender_id, request->data.Agent_query.pwd);
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
            else if (op_result == SQLITE_NOTFOUND)
            {
                response->type = RESP_ERROR;
                strcpy(response->message, "Record no found");
            }
            else
            {
                response->type = RESP_ERROR;
                strcpy(response->message, "Something went wrong on server side");
            }
        }

        // case the password is wrong
        else if (op_result == 1)
        {
            terminal_print(MSG_INFO, "Client tried to lo-in with the wrong password", SERVER, "Server");
            response->type = RESP_AUTH_REQUIRED;
            strcpy(response->message, "Wrong password");
        }

        // errior case
        else if (op_result == -2)
        {
            terminal_print(MSG_ERROR, "Somenthing went wrong while trying to acces the database to get an agent password", SERVER, "Server");
            response->type = RESP_ERROR;
            strcpy(response->message, " Somenthing went wrong on server side");
        }
    
  

    return 0;
}