#include "../../Libs/Client/Graphics_primitives.h"

void draw_label(const char *text, Font font, Vector2 position, int fontSize, Color color)
{
    DrawTextEx(font, text, position , fontSize, 1,color);
}


void draw_text_box(const char *label, const char *text, Vector2 position, 
                   bool active_text_box, Graphical_struct *g_struct, Rectangle *text_box)
{


    // draw the label
    DrawTextEx(g_struct->font_pointer, label,  (Vector2){position.x, position.y + (g_struct->dimensions.textBoxHeight - g_struct->font_sizes.labelFontSize) / 2}, 
               g_struct->font_sizes.labelFontSize, 1, COLOR_LABEL);

    // calculate the rectangle for the text box
    Rectangle box = {
        position.x + g_struct->dimensions.labelWidth,
        position.y,
        g_struct->dimensions.textBoxWidth,
        g_struct->dimensions.textBoxHeight
    };
    *text_box = box;


    // draw the text box background and border
    DrawRectangleRec(box, active_text_box ? COLOR_INPUT_ACTIVE_BG : COLOR_INPUT_BG);
    DrawRectangleLinesEx(box, 2, active_text_box ? COLOR_INPUT_BORDER_ACTIVE : COLOR_INPUT_BORDER);

    // draw the text inside the box
    if (text && strlen(text) > 0)
    {
        DrawTextEx(g_struct->font_pointer, text,
                   (Vector2){box.x + 5, box.y + (g_struct->dimensions.textBoxHeight - g_struct->font_sizes.inputFontSize) / 2},
                   g_struct->font_sizes.inputFontSize, 1, COLOR_INPUT_TEXT);
    }
}



void draw_button(const char *label, Graphical_struct *g_struct, Rectangle *button, int textSize)
{
    // background and border
    DrawRectangleRec(*button, COLOR_BUTTON_BG);
    DrawRectangleLinesEx(*button, 2, COLOR_BUTTON_BORDER);

    // central text 
    int fontSize = textSize;
    Vector2 txtSize = MeasureTextEx(g_struct->font_pointer, label, fontSize, 1);

    DrawTextEx(g_struct->font_pointer,
               label,
               (Vector2){
                   button->x + (button->width - txtSize.x) / 2,
                   button->y + (button->height - txtSize.y) / 2
               },
               fontSize,
               1,
               COLOR_BUTTON_TEXT);
}
