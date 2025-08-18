// Header file containing the grapfical functions related to the standard client process
#ifndef __CLIENT_G__
#define __CLIENT_G__

#include "raylib.h"
#include "../Terminal_com.h"
#include "../Packets.h"
#include "./Graphics_primitives.h"

#define FONT_PATH "../Assets/Roboto.ttf"

typedef enum {
    STATE_WAIT_SPACE,
    STATE_FORM,
    STATE_SENT
} ScreenState;

#define SCALE 0.4f
#define BUTTON_W_RATIO = 0.25f // 25% of the textBoxWidth

// function that init the Client user interface window
// retruns 0 if everithing went well
// retruns -1 if some error occured
int init_client_window(Graphical_struct* client_struct, char * name);

//function that permit the user to chose between the 2 differnt possible request client is allowd to do
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int req_type_graphics(RequestPacket *request, Graphical_struct *g_struct);

// function that create a graphical form for the user for a new ticket 
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int ticket_graphics(RequestPacket *request, Graphical_struct* client_struct);

// function that create the graphical menu to formulate the filters and the populates it
// agente_query_flag basically allows to reused the function specifing which structure use
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int filters_graphics(RequestPacket *request, bool agent_query_flag, Graphical_struct *g_struct);

//function that create the graphical menu to formulate the modification and the populates it
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int modification_graphics(RequestPacket *request, Graphical_struct *g_struct);

// Only function that shows the response and the message
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int response_graphics(ResponsePacket *response, char *name, Graphical_struct *g_struct);

// function that show an error on graphics
// retrun 0 if the user presed next 
// return -1 in case something wrong happen 
int error_graphics(const char *errorMsg, Graphical_struct *g_struct);

// function that close thye screen and everithing related to that
void stop_client_window(Graphical_struct* client_struct, char* name);

#endif