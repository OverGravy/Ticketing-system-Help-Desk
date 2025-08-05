#include "../Libs/Packets.h"
int serialize_request(const RequestPacket* req, char* buffer){
    if (!req || !buffer || BUFFER_SIZE < 1) return -1; // Check for null pointers and buffer size
    
    // Start with an empty buffer
    buffer[0] = '\0';
    size_t offset = 0;
    
    // Serialize the request type
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->type);
    
    // Serialize the role
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->role);

    // Serialize the sender ID
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->sender_id);

    // Serialize the union data based on the request type
    switch (req->type) {
        case REQ_SIGNIN: // Serialize the sing-in request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", req->data.signin.agent_id);
            break;

        case REQ_LOGIN: // Serialize the login request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|%d|", 
                               req->data.login.agent_id, req->data.login.pwd);
            break;

        case REQ_CREATE_TICKET: // Serialize the new ticket request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|%s|%d|%d|", 
                               req->data.new_ticket.title, req->data.new_ticket.description, req->data.new_ticket.priority, req->data.new_ticket.client_id);
            break;

        case REQ_QUERY_AND_MODIFY_TICKET: // Serialize the query and modification request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|%s|%d|%d|%d|%d|",
                               req->data.mod_query.title, req->data.mod_query.description, 
                               req->data.mod_query.client_id, req->data.mod_query.new_priority, 
                               req->data.mod_query.new_status, req->data.mod_query.new_client_id);
            break;

        case REQ_GET_PRIORITY_TICKET: // Serialize the priority ticket request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|%d|", req->data.priority_query.title, req->data.priority_query.client_id);
            break;

        default:
            return -1; // Unknown request type
    }
    
    // Ensure the buffer is null-terminated
    buffer[BUFFER_SIZE - 1] = '\0';
    return 0; // Success

}


// Function to serialize a ResponsePacket into a buffer
int serialize_response(const ResponsePacket* resp, char* buffer) {
    if (!resp || !buffer || BUFFER_SIZE < 1) return -1; // Check for null pointers and buffer size
    
    // Start with an empty buffer
    buffer[0] = '\0';
    size_t offset = 0;

    // Serialize the response type
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", resp->type);

    // Serialize the status code
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|", resp->status_code);

    // Serialize the message
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s|", resp->message);

    // Ensure the buffer is null-terminated
    buffer[BUFFER_SIZE - 1] = '\0';
    return 0; // Success
}

// Function to deserialize a RequestPacket from a buffer
int deserialize_request(const char* buffer, RequestPacket* req) {
    if (!buffer || !req) return -1; // Check for null pointers

    // Reset the request structure
    memset(req, 0, sizeof(RequestPacket));
    
    // Parse the buffer
    char* token;
    char* rest = (char*)buffer; // Create a mutable copy of the buffer for strtok_r

    // Get the request type
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    req->type = (RequestType)atoi(token);
    
    // Get the role
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    req->role = (UserRole)atoi(token);

    // Get the sender ID
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    req->sender_id = atoi(token);

    // Deserialize the union data based on the request type
    switch (req->type) {
        case REQ_SIGNIN: // Deserialize the sing-in request
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.signin.agent_id = atoi(token);             // Get the agent ID
            break;

        case REQ_LOGIN: // Deserialize the login request
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.login.agent_id = atoi(token);               // Get the agent ID
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.login.pwd = atoi(token);                     // Get the password                      
            break;

        case REQ_CREATE_TICKET: // Deserialize the new ticket request
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.new_ticket.title, token, MAX_TITLE_LEN - 1);
            req->data.new_ticket.title[MAX_TITLE_LEN - 1] = '\0';              // title
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.new_ticket.description, token, MAX_DESC_LEN - 1);
            req->data.new_ticket.description[MAX_DESC_LEN - 1] = '\0';          // descption
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.new_ticket.priority = atoi(token);                        // Get the priority
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.new_ticket.client_id = atoi(token);                         // Get the client ID
            break;
            
        case REQ_QUERY_AND_MODIFY_TICKET: // Deserialize the query and modification request
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.mod_query.title, token, MAX_TITLE_LEN - 1);
            req->data.mod_query.title[MAX_TITLE_LEN - 1] = '\0';                 // title
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.mod_query.description, token, MAX_DESC_LEN - 1);
            req->data.mod_query.description[MAX_DESC_LEN - 1] = '\0';            // description

            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.mod_query.client_id = atoi(token);                         // Get the client ID

            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.mod_query.new_priority = atoi(token);                       // Get the new priority
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.mod_query.new_status = (TicketStatus)atoi(token);           // Get the new status
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.mod_query.new_client_id = atoi(token);                      // Get the new client ID
            break;
            
        case REQ_GET_PRIORITY_TICKET: // Deserialize the priority ticket request
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.priority_query.title, token, MAX_TITLE_LEN - 1);
            req->data.priority_query.title[MAX_TITLE_LEN - 1] = '\0';               

            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.priority_query.client_id = atoi(token);
            break;
        default:
            return -1;
    }  
    
    return 0; // Success
    
    
}

int deserialize_response(const char* buffer, ResponsePacket* resp){
     if (!buffer || !resp) return -1; // Check for null pointers

    // Reset the response structure
    memset(resp, 0, sizeof(ResponsePacket));
    
    // Parse the buffer
    char* token;
    char* rest = (char*)buffer; // Create a mutable copy of the buffer for strtok_r

    // Get the response type
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    resp->type = (ResponseType)atoi(token);
    
    // Get the status of the ticket 
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    resp->status_code = (TicketStatus)atoi(token);

    // Get the message
    token = strtok_r(rest, "|", &rest);
    if (!token) return -1;
    strncpy(resp->message, token, MAX_RESP_MSG_LEN -1);
    resp->message[MAX_RESP_MSG_LEN-1] = '\0';

    // everything went fine
    return 0;
}