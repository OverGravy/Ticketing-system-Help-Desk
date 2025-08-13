#include "../../../Libs/Clients/Agent/Agent_graphics.h"

int init_agent_window(int id)
{
    // Step 1: create a small initial window to avoid GLFW warnings
    const int initialWidth = 200;
    const int initialHeight = 200;

    char title[64];
    if (snprintf(title, sizeof(title), "Agent %d", id) < 0)
    {
        return -1; // Error: title formatting failed
    }

    // no need afer debug
    SetTraceLogLevel(LOG_NONE);

    // real init
    InitWindow(initialWidth, initialHeight, title);
    if (!IsWindowReady())
    {
        return -1; // Error: window creation failed
    }

    // Step 2: now that the window exists, get the actual monitor size
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    if (screenWidth <= 0 || screenHeight <= 0)
    {
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

int filters_graphics(RequestPacket *request)
{
    if (!request)
        return -1; // null pointer check

    char message[256];
    char path[256];

    strcpy(path, "../Assets/Roboto.ttf");

    // Load bold font
    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0)
    {
        strcpy(message, "Unable to find .ttf file in: ");
        strcat(message, path);
        terminal_print(MSG_ERROR, message, AGENT, "Agent");
        return -1; // failed to load font
    }

    int activeTextBox = -1;
    bool sent = false;
    bool started = false; // nuova variabile per iniziare la compilazione

    // Temporary buffers for user input
    char ticketIdStr[64] = "";
    char clientIdStr[64] = "";
    char titleStr[128] = "";
    char descriptionStr[256] = "";

    // Length trackers for each text box
    int lenTicket = 0;
    int lenClient = 0;
    int lenTitle = 0;
    int lenDesc = 0;

    while (!sent && !WindowShouldClose())
    {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Dynamic font sizes
        int labelFontSize = screenH / 25;
        int inputFontSize = screenH / 28;
        int buttonFontSize = screenH / 24;
        int titleFontSize = screenH / 17; // un po' più grande

        BeginDrawing();
        ClearBackground(COLOR_BG);

        if (!started)
        {
            // Mostra messaggio "Press SPACE to start..."
            const char *startMsg = "Press SPACE to start creating a new request";
            Vector2 textSize = MeasureTextEx(boldFont, startMsg, buttonFontSize, 1);
            DrawTextEx(boldFont, startMsg,
                       (Vector2){(screenW - textSize.x) / 2, (screenH - textSize.y) / 2},
                       buttonFontSize, 1, COLOR_LABEL);

            // Se è stato premuto SPACE, cambia stato
            if (IsKeyPressed(KEY_SPACE))
            {
                started = true;
            }
        }
        else
        {
            // Layout parameters
            Vector2 startPos = {50, 50};
            int labelWidth = screenW / 6;
            int textBoxWidth = screenW / 2;
            int textBoxHeight = screenH / 18;
            int spacing = textBoxHeight + 20;

            // Disegna titolo in alto a sinistra
            const char *title = "Query Filters:";
            DrawTextEx(boldFont, title,
                       (Vector2){startPos.x, startPos.y},
                       titleFontSize, 1, COLOR_LABEL);

            // Sposta i campi più in basso per lasciare spazio al titolo
            startPos.y += titleFontSize + 35;

            struct
            {
                const char *label;
                char *buffer;
                int maxLen;
                int *length;
            } fields[4] = {
                {"Ticket ID", ticketIdStr, sizeof(ticketIdStr) - 1, &lenTicket},
                {"Client ID", clientIdStr, sizeof(clientIdStr) - 1, &lenClient},
                {"Title", titleStr, sizeof(titleStr) - 1, &lenTitle},
                {"Description", descriptionStr, sizeof(descriptionStr) - 1, &lenDesc}};

            // Draw each field
            for (int i = 0; i < 4; i++)
            {
                int y = startPos.y + i * spacing;

                // Draw label
                DrawTextEx(boldFont, fields[i].label,
                           (Vector2){startPos.x, y + (textBoxHeight - labelFontSize) / 2},
                           labelFontSize, 1, COLOR_LABEL);

                // Text box rectangle
                Rectangle box = {startPos.x + labelWidth, y, textBoxWidth, textBoxHeight};

                // Background color based on active state
                DrawRectangleRec(box, (activeTextBox == i) ? COLOR_INPUT_ACTIVE_BG : COLOR_INPUT_BG);

                // Border color based on active state
                DrawRectangleLinesEx(box, 2, (activeTextBox == i) ? COLOR_INPUT_BORDER_ACTIVE : COLOR_INPUT_BORDER);

                // Draw input text only (no placeholder)
                if (*(fields[i].length) > 0)
                {
                    DrawTextEx(boldFont, fields[i].buffer,
                               (Vector2){box.x + 5, box.y + (textBoxHeight - inputFontSize) / 2},
                               inputFontSize, 1, COLOR_INPUT_TEXT);
                }

                // Activate on click
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 mp = GetMousePosition();
                    if (CheckCollisionPointRec(mp, box))
                    {
                        activeTextBox = i;
                    }
                }

                // Handle keyboard input
                if (activeTextBox == i)
                {
                    int key = GetCharPressed();
                    while (key > 0)
                    {
                        if ((key >= 32) && (key <= 125) && *(fields[i].length) < fields[i].maxLen)
                        {
                            fields[i].buffer[(*(fields[i].length))++] = (char)key;
                            fields[i].buffer[*(fields[i].length)] = '\0';
                        }
                        key = GetCharPressed();
                    }

                    if (IsKeyPressed(KEY_BACKSPACE) && *(fields[i].length) > 0)
                    {
                        fields[i].buffer[--(*(fields[i].length))] = '\0';
                    }
                }
            }

            // SEND button
            Rectangle sendBtn = {
                startPos.x + labelWidth + textBoxWidth - 120,
                startPos.y + spacing * 4, 120, textBoxHeight};
            DrawRectangleRec(sendBtn, COLOR_BUTTON_BG);
            DrawRectangleLinesEx(sendBtn, 2, COLOR_BUTTON_BORDER);
            Vector2 sendTextSize = MeasureTextEx(boldFont, "NEXT", buttonFontSize, 1);
            DrawTextEx(boldFont, "NEXT",
                       (Vector2){sendBtn.x + (sendBtn.width - sendTextSize.x) / 2,
                                 sendBtn.y + (sendBtn.height - sendTextSize.y) / 2},
                       buttonFontSize, 1, COLOR_BUTTON_TEXT);

            // Check send click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, sendBtn))
                {
                    request->data.Agent_query.filters.ticket_id = (lenTicket > 0) ? atoi(ticketIdStr) : INT_UNUSED;
                    request->data.Agent_query.filters.client_id = (lenClient > 0) ? atoi(clientIdStr) : INT_UNUSED;
                    strcpy(request->data.Agent_query.filters.title, (lenTitle > 0) ? titleStr : STR_UNUSED);
                    strcpy(request->data.Agent_query.filters.description, (lenDesc > 0) ? descriptionStr : STR_UNUSED);
                    sent = true;
                }
            }
        }

        EndDrawing();
    }

    // unload the font
    UnloadFont(boldFont);

    // check if the window has been closed
    if (WindowShouldClose())
        return 1;

    return sent ? 0 : -1;
}

int modification_graphics(RequestPacket *request)
{
    if (!request)
        return -1;

    char message[256];
    char path[256];
    strcpy(path, "../Assets/Roboto.ttf");

    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0)
    {
        strcpy(message, "Unable to find .ttf file in: ");
        strcat(message, path);
        terminal_print(MSG_ERROR, message, AGENT, "Agent");
        return -1;
    }

    int activeTextBox = -1;
    bool sent = false;

    char clientIdStr[64] = "";
    char priorityStr[64] = "";
    int lenClient = 0;
    int lenPriority = 0;

    int selectedStatus = -1;

    while (!sent && !WindowShouldClose())
    {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        int titleFontSize = screenH / 17;  // più grande del resto
        int labelFontSize = screenH / 25;
        int inputFontSize = screenH / 28;
        int buttonFontSize = screenH / 24;

        BeginDrawing();
        ClearBackground(COLOR_BG);

        Vector2 startPos = {50, 50};
        int textBoxWidth = screenW / 2;
        int textBoxHeight = screenH / 18;
        int spacing = textBoxHeight + 20;

        // --- Titolo pagina ---
        DrawTextEx(boldFont, "Modification parameters:",
                   startPos, titleFontSize, 1, COLOR_LABEL);

        // --- Label sopra i bottoni status ---
        const char *statusTitle = "New status";
        Vector2 statusTitleSize = MeasureTextEx(boldFont, statusTitle, labelFontSize, 1);
        DrawTextEx(boldFont, statusTitle,
                   (Vector2){(screenW - statusTitleSize.x) / 2, startPos.y + titleFontSize + 20},
                   labelFontSize, 1, COLOR_LABEL);

        // --- Bottoni status ---
        const char *statusLabels[3] = {"Open", "Closed", "In Progress"};
        int numStatus = 3;
        int btnWidth = (screenW - 2 * startPos.x - 40) / numStatus;
        int btnY = startPos.y + titleFontSize + 20 + statusTitleSize.y + 10;

        for (int i = 0; i < numStatus; i++)
        {
            Rectangle btn = {
                startPos.x + i * (btnWidth + 20),
                btnY,
                btnWidth,
                textBoxHeight};

            DrawRectangleRec(btn, (selectedStatus == i) ? COLOR_STATUS_SELECTED : COLOR_STATUS_BG);
            DrawRectangleLinesEx(btn, 2, COLOR_INPUT_BORDER);

            Vector2 txtSize = MeasureTextEx(boldFont, statusLabels[i], buttonFontSize, 1);
            DrawTextEx(boldFont, statusLabels[i],
                       (Vector2){btn.x + (btn.width - txtSize.x) / 2,
                                 btn.y + (btn.height - txtSize.y) / 2},
                       buttonFontSize, 1, COLOR_STATUS_TEXT);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, btn))
                {
                    selectedStatus = i;
                }
            }
        }

        // --- Campi di input ---
        struct
        {
            const char *label;
            char *buffer;
            int maxLen;
            int *length;
        } fields[2] = {
            {"New Client ID", clientIdStr, sizeof(clientIdStr) - 1, &lenClient},
            {"New Priority", priorityStr, sizeof(priorityStr) - 1, &lenPriority}};

        for (int i = 0; i < 2; i++)
        {
            int y = btnY + textBoxHeight + 40 + i * spacing;

            DrawTextEx(boldFont, fields[i].label,
                       (Vector2){startPos.x, y + (textBoxHeight - labelFontSize) / 2},
                       labelFontSize, 1, COLOR_LABEL);

            Rectangle box = {startPos.x + screenW / 6, y, textBoxWidth, textBoxHeight};

            DrawRectangleRec(box, (activeTextBox == i) ? COLOR_INPUT_ACTIVE : COLOR_INPUT_BG);
            DrawRectangleLinesEx(box, 2, (activeTextBox == i) ? COLOR_INPUT_BORDER_ACTIVE : COLOR_INPUT_BORDER);

            DrawTextEx(boldFont, fields[i].buffer,
                       (Vector2){box.x + 5, box.y + (textBoxHeight - inputFontSize) / 2},
                       inputFontSize, 1, COLOR_INPUT_TEXT);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, box))
                {
                    activeTextBox = i;
                }
            }

            if (activeTextBox == i)
            {
                int key = GetCharPressed();
                while (key > 0)
                {
                    if ((key >= 32) && (key <= 125) && *(fields[i].length) < fields[i].maxLen)
                    {
                        fields[i].buffer[(*(fields[i].length))++] = (char)key;
                        fields[i].buffer[*(fields[i].length)] = '\0';
                    }
                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE) && *(fields[i].length) > 0)
                {
                    fields[i].buffer[--(*(fields[i].length))] = '\0';
                }
            }
        }

        // --- Bottone SEND ---
        Rectangle sendBtn = {
            startPos.x + screenW / 6 + textBoxWidth - 120,
            btnY + textBoxHeight + 40 + spacing * 2,
            120, textBoxHeight};

        DrawRectangleRec(sendBtn, COLOR_BUTTON_BG);
        DrawRectangleLinesEx(sendBtn, 2, COLOR_BUTTON_BORDER);

        Vector2 sendTxtSize = MeasureTextEx(boldFont, "SEND", buttonFontSize, 1);
        DrawTextEx(boldFont, "SEND",
                   (Vector2){sendBtn.x + (sendBtn.width - sendTxtSize.x) / 2,
                             sendBtn.y + (sendBtn.height - sendTxtSize.y) / 2},
                   buttonFontSize, 1, COLOR_BUTTON_TEXT);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, sendBtn))
            {
                request->data.Agent_query.mod.new_status = (selectedStatus >= 0) ? selectedStatus : INT_UNUSED;
                request->data.Agent_query.mod.new_client_id = (lenClient > 0) ? atoi(clientIdStr) : INT_UNUSED;
                request->data.Agent_query.mod.new_priority = (lenPriority > 0) ? atoi(priorityStr) : INT_UNUSED;
                sent = true;
            }
        }

        EndDrawing();
    }

    UnloadFont(boldFont);

    if (WindowShouldClose())
        return 1;

    return sent ? 0 : -1;
}


int response_graphics(ResponsePacket *response, char *name)
{
    char terminal_message[256];

    if (!response)
        return -1; // Null pointer check

    char path[256];
    strcpy(path, "../Assets/Roboto.ttf");

    // Load bold font
    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0)
    {
        terminal_print(MSG_ERROR, "Unable to find .ttf file in: ../Assets/Roboto.ttf", AGENT, name);
        return -1;
    }

    bool closed = false;
    bool buttonPressed = false;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Font sizes
    int mainFontSize = screenH / 15;
    int messageFontSize = screenH / 30;
    int buttonFontSize = screenH / 24; // coerente con altre funzioni

    // show main message
    const char *mainMsg = NULL;
    Color mainColor = COLOR_INPUT_TEXT;

    // write on the console what the server says
    strcpy(terminal_message, "Server says: ");
    strcat(terminal_message, response->message);

    switch (response->type)
    {
    case RESP_QUERY_MOD_OK:
        mainMsg = "Success";
        mainColor = (Color){0, 255, 0, 255}; // green
        terminal_print(MSG_INFO, terminal_message, AGENT, name);
        terminal_print(MSG_SUCCESS, "Query and modification went fine", AGENT, name);
        break;
    case RESP_ERROR:
        mainMsg = "Failed";
        mainColor = (Color){255, 0, 0, 255}; // red
        terminal_print(MSG_INFO, terminal_message, AGENT, name);
        terminal_print(MSG_ERROR, "Query and modification failed", AGENT, name);
        break;
    default:
        mainMsg = "";
        mainColor = COLOR_INPUT_TEXT;
        break;
    }

    // Define button
    int buttonWidth = screenW / 8;
    int buttonHeight = screenH / 14;
    Rectangle okButton = {
        (screenW - buttonWidth) / 2,
        (screenH / 2) + (screenH / 6),
        buttonWidth,
        buttonHeight};

    // Main loop
    while (!closed && !buttonPressed && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Draw main message
        if (mainMsg && mainMsg[0] != '\0')
        {
            Vector2 mainSize = MeasureTextEx(boldFont, mainMsg, mainFontSize, 1);
            DrawTextEx(boldFont, mainMsg,
                       (Vector2){(screenW - mainSize.x) / 2, (screenH / 2) - mainSize.y},
                       mainFontSize, 1, mainColor);
        }

        // Draw the message contained in the response
        if (response->message[0] != '\0')
        {
            Vector2 msgSize = MeasureTextEx(boldFont, response->message, messageFontSize, 1);
            int yPos = (screenH / 2) + 10;
            DrawTextEx(boldFont, response->message,
                       (Vector2){(screenW - msgSize.x) / 2, yPos},
                       messageFontSize, 1, COLOR_INPUT_TEXT);
        }

        // Draw OK button (coerente con altre schermate)
        DrawRectangleRec(okButton, COLOR_BUTTON_BG);
        DrawRectangleLinesEx(okButton, 2, COLOR_BUTTON_BORDER);

        Vector2 btnTextSize = MeasureTextEx(boldFont, "OK", buttonFontSize, 1);
        DrawTextEx(boldFont, "OK",
                   (Vector2){okButton.x + (okButton.width - btnTextSize.x) / 2,
                             okButton.y + (okButton.height - btnTextSize.y) / 2},
                   buttonFontSize, 1, COLOR_BUTTON_TEXT);

        EndDrawing();

        // Button click detection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, okButton))
            {
                buttonPressed = true;
            }
        }
    }

    UnloadFont(boldFont);

    if (WindowShouldClose())
        return 1; // Window closed
    if (buttonPressed)
        return 0; // Button pressed

    return -1; // Something went wrong
}




int no_filter_graphics(char * name)
{
    char path[256];
    strcpy(path, "../Assets/Roboto.ttf");

    // Load bold font
    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0)
    {
        terminal_print(MSG_ERROR, "Unable to find .ttf file in: ../Assets/Roboto.ttf", AGENT, name);
        return -1;
    }

    bool closed = false;
    bool buttonPressed = false;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Font sizes coerenti
    int mainFontSize = screenH / 15;
    int messageFontSize = screenH / 30;
    int buttonFontSize = screenH / 24;

    // Define messages
    const char *mainMsg = "Error";
    const char *subMsg = "No filter selected";
    Color mainColor = (Color){255, 0, 0, 255}; // Red

    // Define button
    int buttonWidth = screenW / 8;
    int buttonHeight = screenH / 14;
    Rectangle okButton = {
        (screenW - buttonWidth) / 2,
        (screenH / 2) + (screenH / 6),
        buttonWidth,
        buttonHeight};

    // Main loop
    while (!closed && !buttonPressed && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Draw main message (Error)
        Vector2 mainSize = MeasureTextEx(boldFont, mainMsg, mainFontSize, 1);
        DrawTextEx(boldFont, mainMsg,
                   (Vector2){(screenW - mainSize.x) / 2, (screenH / 2) - mainSize.y},
                   mainFontSize, 1, mainColor);

        // Draw sub-message
        Vector2 msgSize = MeasureTextEx(boldFont, subMsg, messageFontSize, 1);
        int yPos = (screenH / 2) + 10;
        DrawTextEx(boldFont, subMsg,
                   (Vector2){(screenW - msgSize.x) / 2, yPos},
                   messageFontSize, 1, COLOR_INPUT_TEXT);

        // Draw OK button
        DrawRectangleRec(okButton, COLOR_BUTTON_BG);
        DrawRectangleLinesEx(okButton, 2, COLOR_BUTTON_BORDER);
        Vector2 btnTextSize = MeasureTextEx(boldFont, "OK", buttonFontSize, 1);
        DrawTextEx(boldFont, "OK",
                   (Vector2){okButton.x + (okButton.width - btnTextSize.x) / 2,
                             okButton.y + (okButton.height - btnTextSize.y) / 2},
                   buttonFontSize, 1, COLOR_BUTTON_TEXT);

        EndDrawing();

        // Button click detection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, okButton))
            {
                buttonPressed = true;
            }
        }
    }

    UnloadFont(boldFont);

    if (WindowShouldClose())
        return 1; // Window closed
    if (buttonPressed)
        return 0; // Button pressed

    return -1; // Something went wrong
}


int no_mod_graphics(char * name)
{
    char path[256];
    strcpy(path, "../Assets/Roboto.ttf");

    // Load bold font
    Font boldFont = LoadFont(path);
    if (boldFont.texture.id == 0)
    {
        terminal_print(MSG_ERROR, "Unable to find .ttf file in: ../Assets/Roboto.ttf", AGENT, name);
        return -1;
    }

    bool closed = false;
    bool buttonPressed = false;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Font sizes coerenti
    int mainFontSize = screenH / 15;
    int messageFontSize = screenH / 30;
    int buttonFontSize = screenH / 24;

    // Define messages
    const char *mainMsg = "Error";
    const char *subMsg = "No modification applied";
    Color mainColor = (Color){255, 0, 0, 255}; 
    // Define button
    int buttonWidth = screenW / 8;
    int buttonHeight = screenH / 14;
    Rectangle okButton = {
        (screenW - buttonWidth) / 2,
        (screenH / 2) + (screenH / 6),
        buttonWidth,
        buttonHeight};

    // Main loop
    while (!closed && !buttonPressed && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Draw main message (Error)
        Vector2 mainSize = MeasureTextEx(boldFont, mainMsg, mainFontSize, 1);
        DrawTextEx(boldFont, mainMsg,
                   (Vector2){(screenW - mainSize.x) / 2, (screenH / 2) - mainSize.y},
                   mainFontSize, 1, mainColor);

        // Draw sub-message
        Vector2 msgSize = MeasureTextEx(boldFont, subMsg, messageFontSize, 1);
        int yPos = (screenH / 2) + 10;
        DrawTextEx(boldFont, subMsg,
                   (Vector2){(screenW - msgSize.x) / 2, yPos},
                   messageFontSize, 1, COLOR_INPUT_TEXT);

        // Draw OK button
        DrawRectangleRec(okButton, COLOR_BUTTON_BG);
        DrawRectangleLinesEx(okButton, 2, COLOR_BUTTON_BORDER);
        Vector2 btnTextSize = MeasureTextEx(boldFont, "OK", buttonFontSize, 1);
        DrawTextEx(boldFont, "OK",
                   (Vector2){okButton.x + (okButton.width - btnTextSize.x) / 2,
                             okButton.y + (okButton.height - btnTextSize.y) / 2},
                   buttonFontSize, 1, COLOR_BUTTON_TEXT);

        EndDrawing();

        // Button click detection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, okButton))
            {
                buttonPressed = true;
            }
        }
    }

    UnloadFont(boldFont);

    if (WindowShouldClose())
        return 1; // Window closed
    if (buttonPressed)
        return 0; // Button pressed

    return -1; // Something went wrong
}