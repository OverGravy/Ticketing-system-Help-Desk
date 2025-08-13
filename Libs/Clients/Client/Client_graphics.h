// Header file containing the grapfical functions related to the standard client process
#ifndef __CLIENT_G__
#define __CLIENT_G__

#include "raylib.h"
#include "../../Terminal_com.h"
#include "../../Packets.h"

#define COLOR_BG                 RAYWHITE

#define COLOR_LABEL              BLACK
#define COLOR_INPUT_TEXT         BLACK

#define COLOR_INPUT_BG           GRAY
#define COLOR_INPUT_ACTIVE_BG    LIGHTGRAY
#define COLOR_INPUT_BORDER       DARKGRAY
#define COLOR_INPUT_BORDER_ACTIVE BLUE

#define COLOR_BUTTON_BG          BLUE
#define COLOR_BUTTON_TEXT        WHITE
#define COLOR_BUTTON_BORDER      BLUE

#define COLOR_STATUS_BG          LIGHTGRAY
#define COLOR_STATUS_SELECTED_BG BLUE
#define COLOR_STATUS_TEXT        BLACK

#define COLOR_WAIT_TEXT          DARKGRAY

#define COLOR_PLACEHOLDER (Color){150, 150, 150, 255}  
#define COLOR_STATUS_SELECTED (Color){0, 122, 204, 255}  
#define COLOR_INPUT_ACTIVE    (Color){220, 220, 220, 255} 


#define SCALE 0.4f

typedef enum {
    STATE_WAIT_SPACE,
    STATE_FORM,
    STATE_SENT
} ScreenState;

// function that init the Client user interface window
// retruns 0 if everithing went well
// retruns -1 if some error occured
int init_client_window(int id);

//function that permit the user to chose between the 2 differnt possible request client is allowd to do
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int req_type_graphics(RequestPacket *request);

// function that create a graphical form for the user for a new ticket 
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int ticket_graphics(RequestPacket *request);

// function that create the graphical menu to formulate the filters and the populates it
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int filters_graphics(RequestPacket *request);

// Only function that shows the response and the message
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int response_graphics(ResponsePacket *response, char* name);

#endif