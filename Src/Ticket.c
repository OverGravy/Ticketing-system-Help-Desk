#include "../Libs/Ticket.h"

// This file implements the serialization and deserialization functions for the Ticket structure.
int serialize_packet(const struct Packet* p, char* buffer, size_t buf_size){
    if (!p || !p->contenct.creation_date || !buffer) return -1; // Check for null pointers

    int written = snprintf(buffer, buf_size, "%d|%s|%s|%02d/%02d/%04d|%d|%d|%d",
        p->type,
        p->contenct.title,
        p->contenct.description,
        p->contenct.creation_date->day,
        p->contenct.creation_date->month,
        p->contenct.creation_date->year,
        p->contenct.priority,
        p->contenct.status,
        p->contenct.agent_id
    );

    return (written < 0 || (size_t)written >= buf_size) ? -1 : 0; // return 0 if everithing is ok, -1 if there was an error
}


// function that deserialize a packet from a buffer
int deserialize_packet(const char* buffer, struct Packet* p) {
    if (!buffer || !p) return -1;   // Check for null pointers

    char temp[256];
    strncpy(temp, buffer, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* token = strtok(temp, "|");
    if (!token) return -1;
    p->type = atoi(token);

    // Title
    token = strtok(NULL, "|");
    if (!token) return -1;
    strncpy(p->contenct.title, token, sizeof(p->contenct.title));
    p->contenct.title[sizeof(p->contenct.title) - 1] = '\0';

    // Description
    token = strtok(NULL, "|");
    if (!token) return -1;
    strncpy(p->contenct.description, token, sizeof(p->contenct.description));
    p->contenct.description[sizeof(p->contenct.description) - 1] = '\0';

    // Date
    token = strtok(NULL, "|");
    if (!token) return -1;
    p->contenct.creation_date = malloc(sizeof(struct Date));
    if (!p->contenct.creation_date) return -1;
    if (sscanf(token, "%d/%d/%d", 
            &p->contenct.creation_date->day,
            &p->contenct.creation_date->month,
            &p->contenct.creation_date->year) != 3) {
        free(p->contenct.creation_date);
        return -1;
    }

    // Priority
    token = strtok(NULL, "|");
    if (!token) return -1;
    p->contenct.priority = atoi(token);

    // Status
    token = strtok(NULL, "|");
    if (!token) return -1;
    p->contenct.status = atoi(token);

    // Agent ID
    token = strtok(NULL, "|");
    if (!token) return -1;
    p->contenct.agent_id = atoi(token);

    return 0;
}