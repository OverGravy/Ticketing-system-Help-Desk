#include "../../../Libs/Clients/Agent/Agent_graphics.h"

int init_agent_window(int id)
{
    // Step 1: create a small initial window to avoid GLFW warnings
    const int initialWidth = 200;
    const int initialHeight = 200;

    char title[64];
    if (snprintf(title, sizeof(title), "Agent %d", id) < 0) {
        return -1; // Error: title formatting failed
    }

    // no need afer debug
    SetTraceLogLevel(LOG_NONE);

    // real init
    InitWindow(initialWidth, initialHeight, title);
    if (!IsWindowReady()) {
        return -1; // Error: window creation failed
    }

    // Step 2: now that the window exists, get the actual monitor size
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    if (screenWidth <= 0 || screenHeight <= 0) {
        return -1; // Error: invalid monitor size
    }

    // Step 3: compute final size (30% of monitor)
    int windowWidth = (int)(screenWidth * SCALE);
    int windowHeight = (int)(screenHeight * SCALE);

    // Step 4: resize and center window
    SetWindowSize(windowWidth, windowHeight);
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;
    SetWindowPosition(posX, posY);

    return 0; // Success
}


int request_graphics(RequestPacket *request) {
    if (!request) return -1; // null pointer check

    char message[256];
    char path[256];

    strcpy(path, "../Assets/Roboto.ttf");

    // Load bold font
    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0) {
        strcpy(message, "Unabel to find .ttf file in: ");
        strcat(message, path);
        terminal_print(MSG_ERROR, message, AGENT, "Agent");
        return -1; // failed to load font
    }

    ScreenState state = STATE_WAIT_SPACE;
    int activeTextBox = -1;
    bool sent = false;

    // Temporary buffers for user input
    char ticketIdStr[64] = "";
    char clientIdStr[64] = "";
    char titleStr[128] = "";
    char descriptionStr[256] = "";

    // Length trackers for each text box
    int lenTicket = 0;
    int lenClient = 0;
    int lenTitle  = 0;
    int lenDesc   = 0;

    while (!sent && !WindowShouldClose()) {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Dynamic font sizes
        int labelFontSize = screenH / 25;
        int inputFontSize = screenH / 28;
        int buttonFontSize = screenH / 24;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (state == STATE_WAIT_SPACE) {
            const char *msg = "Press SPACE to begin a new request";
            Vector2 textSize = MeasureTextEx(boldFont, msg, labelFontSize, 1);
            DrawTextEx(boldFont, msg,
                       (Vector2){ (screenW - textSize.x) / 2, (screenH - textSize.y) / 2 },
                       labelFontSize, 1, DARKGRAY);

            if (IsKeyPressed(KEY_SPACE)) {
                state = STATE_FORM;
            }
        }
        else if (state == STATE_FORM) {
            // Layout parameters
            Vector2 startPos = {50, 50};
            int labelWidth = screenW / 6;
            int textBoxWidth = screenW / 2;
            int textBoxHeight = screenH / 18;
            int spacing = textBoxHeight + 20;

            struct {
                const char* label;
                char* buffer;
                int maxLen;
                int* length;
            } fields[4] = {
                {"Ticket ID",    ticketIdStr,    sizeof(ticketIdStr)-1,    &lenTicket},
                {"Client ID",    clientIdStr,    sizeof(clientIdStr)-1,    &lenClient},
                {"Title",        titleStr,       sizeof(titleStr)-1,       &lenTitle},
                {"Description",  descriptionStr, sizeof(descriptionStr)-1, &lenDesc}
            };

            // Draw each field
            for (int i = 0; i < 4; i++) {
                int y = startPos.y + i * spacing;

                // Draw label
                DrawTextEx(boldFont, fields[i].label,
                           (Vector2){startPos.x, y + (textBoxHeight - labelFontSize) / 2},
                           labelFontSize, 1, BLACK);

                // Text box rectangle
                Rectangle box = {startPos.x + labelWidth, y, textBoxWidth, textBoxHeight};

                // Draw background gray
                DrawRectangleRec(box, (activeTextBox == i) ? LIGHTGRAY : GRAY);
                DrawRectangleLinesEx(box, 2, (activeTextBox == i) ? BLUE : DARKGRAY);

                // Draw input text
                DrawTextEx(boldFont, fields[i].buffer,
                           (Vector2){box.x + 5, box.y + (textBoxHeight - inputFontSize) / 2},
                           inputFontSize, 1, BLACK);

                // Activate on click
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mp = GetMousePosition();
                    if (CheckCollisionPointRec(mp, box)) {
                        activeTextBox = i;
                    }
                }

                // Handle keyboard input
                if (activeTextBox == i) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 32) && (key <= 125) && *(fields[i].length) < fields[i].maxLen) {
                            fields[i].buffer[(*(fields[i].length))++] = (char)key;
                            fields[i].buffer[*(fields[i].length)] = '\0';
                        }
                        key = GetCharPressed();
                    }

                    if (IsKeyPressed(KEY_BACKSPACE) && *(fields[i].length) > 0) {
                        fields[i].buffer[--(*(fields[i].length))] = '\0';
                    }
                }
            }

            // SEND button
            Rectangle sendBtn = {
                startPos.x + labelWidth + textBoxWidth - 120,
                startPos.y + spacing * 4, 120, textBoxHeight
            };
            DrawRectangleRec(sendBtn, BLUE);
            DrawRectangleLinesEx(sendBtn, 2, BLUE);
            Vector2 sendTextSize = MeasureTextEx(boldFont, "SEND", buttonFontSize, 1);
            DrawTextEx(boldFont, "SEND",
                       (Vector2){ sendBtn.x + (sendBtn.width - sendTextSize.x) / 2,
                                  sendBtn.y + (sendBtn.height - sendTextSize.y) / 2 },
                       buttonFontSize, 1, WHITE);

            // Check send click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, sendBtn)) {
                    request->data.Agent_query.filters.ticket_id = (lenTicket > 0) ? atoi(ticketIdStr)  : INT_UNUSED;
                    request->data.Agent_query.filters.client_id = (lenClient > 0) ? atoi(clientIdStr)  : INT_UNUSED;
                    strcpy(request->data.Agent_query.filters.title,       (lenTitle > 0) ? titleStr       : STR_UNUSED);
                    strcpy(request->data.Agent_query.filters.description, (lenDesc  > 0) ? descriptionStr : STR_UNUSED);
                    sent = true;
                }
            }
        }

        EndDrawing();
    }

    // unload the font
    UnloadFont(boldFont);

    // check if the windows has been closed
    if (WindowShouldClose()) return 1;

    return sent ? 0 : -1;
}