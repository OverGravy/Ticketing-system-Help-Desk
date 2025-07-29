#include "../Libs/Ticket.h"

int serialize_packet(const struct Packet* p, char* buffer, size_t buf_size){
    if (!p || !p->contenct.creation_date || !buffer) return -1;

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

    return (written < 0 || (size_t)written >= buf_size) ? -1 : 0;
}
