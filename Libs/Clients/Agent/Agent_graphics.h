// Header file containing the grapfical functions related to the agent process
#ifndef __AGENT_G__
#define __AGENT_G__

#include <string.h>
#include <stdio.h>

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

// function that init the Agent user interface window
// retruns 0 if everithing went well
// retruns -1 if some error occured
int init_agent_window(int id);

// function that create the graphical menu to formulate the filters and the populates it
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int filters_graphics(RequestPacket *request);

//function that create the graphical menu to formulate the modification and the populates it
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int modification_graphics(RequestPacket *request);

//function that print the response 
// retrun 0 if everithing went wll
// retrun 1 if the user cloeses the window
int response_graphics(ResponsePacket *response, char* name);

// function that print an error if the user haven't put any filter
// retrun 0 if everithing went wll
// retrun 1 if the user cloeses the window
int no_filter_graphics(char * name);

// function that print an error if the user haven't put any modifciation
// retrun 0 if everithing went well
// retrun 1 if the user cloeses the window
int no_mod_graphics(char * name);

#endif