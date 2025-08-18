#ifndef __GRAPHICS_PRIMITIVES_H__
#define __GRAPHICS_PRIMITIVES_H__

#include "raylib.h"
#include <string.h>

// Stub define colors and sizes
#define COLOR_BG RAYWHITE

#define COLOR_LABEL BLACK
#define COLOR_INPUT_TEXT BLACK

#define COLOR_INPUT_BG GRAY
#define COLOR_INPUT_ACTIVE_BG LIGHTGRAY
#define COLOR_INPUT_BORDER DARKGRAY
#define COLOR_INPUT_BORDER_ACTIVE BLUE

#define COLOR_BUTTON_BG BLUE
#define COLOR_BUTTON_TEXT WHITE
#define COLOR_BUTTON_BORDER BLUE

#define COLOR_STATUS_BG LIGHTGRAY
#define COLOR_STATUS_SELECTED_BG BLUE
#define COLOR_STATUS_TEXT BLACK

#define COLOR_WAIT_TEXT DARKGRAY

#define COLOR_PLACEHOLDER (Color){150, 150, 150, 255}
#define COLOR_STATUS_SELECTED (Color){0, 122, 204, 255}
#define COLOR_INPUT_ACTIVE (Color){220, 220, 220, 255}

// struct that define differents font sizes that need to be different considering the window size
typedef struct
{
    int labelFontSize;
    int inputFontSize;
    int titleFontSize;
    int formButtonFontSize;
    int menuButtonFontSize;
} Font_sizes;

typedef struct
{
    int labelWidth;          // label width 
    int textBoxWidth;        // width of the textbox
    int textBoxHeight;       // height of the textbox
    int formButtonHeight;    // height of the form button
    int formButtonWidth;     // width of the form button
    int menuButtonHeight;    // height of big menu button
    int menuButtonWidth;     // width of big menu button
} Dimensions;

// defnine a structure to hold graphical
typedef struct
{
    Font font_pointer;            
    char widnow_title[256];  // fixed string dimension
    Font_sizes font_sizes;   
    Dimensions dimensions;   
} Graphical_struct;

// function that draws a label type of text, with a specified position
void draw_label(const char *text, Font font, Vector2 position, int fontSize, Color color);

// function that draws a text box with a label, with a specified position, function need a pointer to the rectangle
// the rectangle is needed for the click collision 
void draw_text_box(const char *label, const char *text, Vector2 position, bool active_text_box, Graphical_struct *g_struct, Rectangle *text_box);

// function that draws a button with a label, with a specified position
void draw_button(const char *label, Graphical_struct *g_struct, Rectangle *button, int textSize);

#endif