#include "../Libs/Packets.h"
int serialize_request(const RequestPacket *req, char *buffer)
{
    if (!req || !buffer || BUFFER_SIZE < 1)
        return -1; // Check for null pointers and buffer size

    // Start with an empty buffer
    buffer[0] = '\0';
    size_t offset = 0;

    // Serialize the request type
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->type);

    // Serialize the sender ID
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->sender_id);

    // Serialize the union data based on the request type
    switch (req->type)
    {

    case REQ_SIGNIN: // serialize the sing in request
        buffer[BUFFER_SIZE - 1] = '\0';
        return 0; // Success

        break;

    case REQ_CREATE_TICKET: // Serialize the new ticket request
        offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|%s|%d|%d|",
                           req->data.new_ticket.title, req->data.new_ticket.description, req->data.new_ticket.priority, req->data.new_ticket.client_id);
        break;

    case REQ_QUERY: // Serialize the query and modification request
        offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|%s|%s|%d|",
                           req->data.Client_query.ticket_id, req->data.Client_query.title,
                           req->data.Client_query.description, req->data.Client_query.client_id);
        break;

    case REQ_QUERY_AND_MOD: // Serialize the query and modification request
        offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|%d|%s|%s|%d|%d|%d|%d|",
                           req->data.Agent_query.pwd, req->data.Agent_query.filters.ticket_id, req->data.Agent_query.filters.title,
                           req->data.Agent_query.filters.description, req->data.Agent_query.filters.client_id,
                           req->data.Agent_query.mod.new_client_id, req->data.Agent_query.mod.new_priority,
                           req->data.Agent_query.mod.new_status);
        break;

    default:
        return -1; // Unknown request type
    }

    // Ensure the buffer is null-terminated
    buffer[BUFFER_SIZE - 1] = '\0';
    return 0; // Success
}

// Function to serialize a ResponsePacket into a buffer
int serialize_response(const ResponsePacket *resp, char *buffer)
{
    if (!resp || !buffer || BUFFER_SIZE < 1)
        return -1; // Check for null pointers and buffer size

    // Start with an empty buffer
    buffer[0] = '\0';
    size_t offset = 0;

    // Serialize the response type
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", resp->type);

    // Serialize the message
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|", resp->message);

    // Ensure the buffer is null-terminated
    buffer[BUFFER_SIZE - 1] = '\0';

    return 0; // Success
}

// Function to deserialize a RequestPacket from a buffer
int deserialize_request(const char *buffer, RequestPacket *req)
{

    if (!buffer || !req)
        return -1; // Check for null pointers

    // Reset the request structure
    memset(req, 0, sizeof(RequestPacket));

    // Parse the buffer
    char *token;
    char *rest = (char *)buffer; // Create a mutable copy of the buffer for strtok_r

    // Get the request type
    token = strtok_r(rest, "|", &rest);
    if (!token)
        return -1;
    req->type = (RequestType)atoi(token);

    // Get the sender ID
    token = strtok_r(rest, "|", &rest);
    if (!token)
        return -1;
    req->sender_id = atoi(token);

    // Deserialize the union data based on the request type
    switch (req->type)
    {

    case REQ_SIGNIN: // Deserialize the sing in request

        return 0; // nothing special to do

        break;

    case REQ_CREATE_TICKET: // Deserialize the new ticket request
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.new_ticket.title, token, MAX_TITLE_LEN - 1);
        req->data.new_ticket.title[MAX_TITLE_LEN - 1] = '\0'; // title

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.new_ticket.description, token, MAX_DESC_LEN - 1);
        req->data.new_ticket.description[MAX_DESC_LEN - 1] = '\0'; // descption

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.new_ticket.priority = atoi(token); // Get the priority

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.new_ticket.client_id = atoi(token); // Get the client ID
        break;

    case REQ_QUERY: // Deserialize the query
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Client_query.ticket_id = atoi(token);

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.Client_query.title, token, MAX_TITLE_LEN - 1);
        req->data.Client_query.title[MAX_TITLE_LEN - 1] = '\0'; // title

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.Client_query.description, token, MAX_DESC_LEN - 1);
        req->data.Client_query.description[MAX_DESC_LEN - 1] = '\0'; // description

        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Client_query.client_id = atoi(token); // Get the client id

        break;

    case REQ_QUERY_AND_MOD: // Deserialize the priority ticket request

        // LOG-IN INFO
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.pwd = atoi(token); // pwd to log-in

        // FILTERS

        // ticket id
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.filters.ticket_id = atoi(token); // ticket-id

        // title
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.Agent_query.filters.title, token, MAX_TITLE_LEN - 1);
        req->data.Agent_query.filters.title[MAX_TITLE_LEN - 1] = '\0'; // title

        // description
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        strncpy(req->data.Agent_query.filters.description, token, MAX_DESC_LEN - 1);
        req->data.Agent_query.filters.description[MAX_DESC_LEN - 1] = '\0'; // description

        // client_id
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.filters.client_id = atoi(token); // Get the client id

        // MODIFICATION

        // new_client_id
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.mod.new_client_id = atoi(token);

        // new_priority
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.mod.new_priority = atoi(token);

        // new_status
        token = strtok_r(rest, "|", &rest);
        if (!token)
            return -1;
        req->data.Agent_query.mod.new_status = atoi(token);

        break;
    default:
        return -1;
    }

    return 0; // Success
}

int deserialize_response(const char *buffer, ResponsePacket *resp)
{
    if (!buffer || !resp)
    {
        return -1;
    } // Check for null pointers

    // Reset the response structure
    memset(resp, 0, sizeof(ResponsePacket));

    // Parse the buffer
    char *token;
    char *rest = (char *)buffer; // Create a mutable copy of the buffer for strtok_r

    // Get the response type
    token = strtok_r(rest, "|", &rest);
    if (!token)
    {
        return -1;
    }
    resp->type = (ResponseType)atoi(token);

    // Get the message
    token = strtok_r(rest, "|", &rest);
    if (!token)
    {

        return -1;
    }
    strncpy(resp->message, token, MAX_RESP_MSG_LEN - 1);
    resp->message[MAX_RESP_MSG_LEN - 1] = '\0';

    // everything went fine
    return 0;
}
