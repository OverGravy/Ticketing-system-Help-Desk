#ifndef __AGENT_G__
#define __AGENT_G__

#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "../../Terminal_com.h"
#include "../../Packets.h"

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

// function that create the graphical menu to formulate the request and the populates it
// retrun -1 if an error occured while compiling the request
// retrun 0 if the request have been populated
// retrun 1 if the user closed the window
int request_graphics(RequestPacket *request);

#endif