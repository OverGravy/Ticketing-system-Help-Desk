// Server request handler header file, it contains the logic that handle the request riceved by the server

#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <string.h>
#include "Tickets_list.h"

int Ticket_handler(int client_fd, struct TicketNode *head, const int buffer_size);

#endif