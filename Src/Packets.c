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

        case REQ_QUERY: // Serialize the query and modification request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|%s|%s|%d|",
                               req->data.filters.ticket_id , req->data.filters.title, 
                               req->data.filters.description , req->data.filters.client_id);
            break;

          case REQ_QUERY_AND_MOD: // Serialize the query and modification request
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%d|%s|%s|%d|%s|%s|%d|%d|%d|",
                               req->data.filters.ticket_id , req->data.filters.title, 
                               req->data.filters.description , req->data.filters.client_id,
                               req->data.modification.new_title, req->data.modification.new_description, 
                               req->data.modification.new_client_id, req->data.modification.new_priority, 
                               req->data.modification.new_status);
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
            
        case REQ_QUERY: // Deserialize the query and modification request
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.filters.ticket_id = atoi(token);    

            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.filters.title, token, MAX_TITLE_LEN - 1);
            req->data.filters.title[MAX_TITLE_LEN - 1] = '\0';                 // title
            
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.filters.description, token, MAX_DESC_LEN - 1);
            req->data.filters.description[MAX_DESC_LEN - 1] = '\0';            // description

            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.filters.client_id = atoi(token);                         // Get the client id              
            break;
            
        case REQ_QUERY_AND_MOD: // Deserialize the priority ticket request
            // FILTERS
        
            // ticket id
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.filters.ticket_id = atoi(token);    

            // title
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.filters.title, token, MAX_TITLE_LEN - 1);
            req->data.filters.title[MAX_TITLE_LEN - 1] = '\0';                 // title
            
            // description
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.filters.description, token, MAX_DESC_LEN - 1);
            req->data.filters.description[MAX_DESC_LEN - 1] = '\0';            // description

            // client_id
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.filters.client_id = atoi(token);                         // Get the client id              

            // MODIFICATION

            // new_title
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.modification.new_title, token, MAX_TITLE_LEN - 1);
            req->data.modification.new_title[MAX_TITLE_LEN - 1] = '\0';   

            // new_description
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            strncpy(req->data.modification.new_description, token, MAX_TITLE_LEN - 1);
            req->data.modification.new_description[MAX_TITLE_LEN - 1] = '\0';   

            // new_client_id
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.modification.new_client_id = atoi(token);    

            // new_priority
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.modification.new_priority = atoi(token);    

            // new_status
            token = strtok_r(rest, "|", &rest);
            if (!token) return -1;
            req->data.modification.new_status = atoi(token);    

            break;
        default:
            return -1;
    }  
    
    return 0; // Success
    
    
}

int deserialize_response(const char* buffer, ResponsePacket* resp){
    if (!buffer || !resp){printf("yo2");return -1;} // Check for null pointers

    // Reset the response structure
    memset(resp, 0, sizeof(ResponsePacket));
    
    // Parse the buffer
    char* token;
    char* rest = (char*)buffer; // Create a mutable copy of the buffer for strtok_r

    // Get the response type
    token = strtok_r(rest, "|", &rest);
    if (!token) {printf("yo");return -1;}
    resp->type = (ResponseType)atoi(token);

    // Get the message
    token = strtok_r(rest, "|", &rest);
    if (!token) {printf("yo1");return -1;}
    strncpy(resp->message, token, MAX_RESP_MSG_LEN -1);
    resp->message[MAX_RESP_MSG_LEN-1] = '\0';

    // everything went fine
    return 0;
}


int reset_response(ResponsePacket* resp){
    resp->type = -1;  
}