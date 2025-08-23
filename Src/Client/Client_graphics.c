#include "../../Libs/Client/Client_graphics.h"

int init_client_window(Graphical_struct *client_struct, char *name)
{

    // Step 1: create a small initial window to avoid GLFW warnings
    const int initialWidth = 200;
    const int initialHeight = 200;
    char message[256]; // error message

    // no need afer debug
    SetTraceLogLevel(LOG_NONE);

    // real init
    InitWindow(initialWidth, initialHeight, client_struct->widnow_title);
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

    // Step 5: load bold font
    client_struct->font_pointer = LoadFont(FONT_PATH);
    if (client_struct->font_pointer.texture.id == 0)
    {
        strcpy(message, "Unable to find .ttf file in: ");
        strcat(message, FONT_PATH);
        terminal_print(MSG_ERROR, message, CLIENT, name);
        return -1; // failed to load font
    }

    // Step 6: adjust font sizes with screen dimension

    windowWidth = GetScreenWidth();
    windowHeight = GetScreenHeight();

    client_struct->font_sizes.labelFontSize = windowHeight / 25;
    client_struct->font_sizes.inputFontSize = windowHeight / 28;
    client_struct->font_sizes.formButtonFontSize = windowHeight / 24;
    client_struct->font_sizes.titleFontSize = windowHeight / 17;
    client_struct->font_sizes.menuButtonFontSize = windowHeight / 18;

    // Step 7: adjust dimensions of pimitives elements dinamically with screen dimension
    client_struct->dimensions.labelWidth = windowWidth / 6;
    client_struct->dimensions.textBoxWidth = windowWidth / 2;
    client_struct->dimensions.textBoxHeight = windowHeight / 18;
    client_struct->dimensions.formButtonHeight = windowHeight / 18;
    client_struct->dimensions.formButtonWidth = windowWidth / 8;
    client_struct->dimensions.menuButtonHeight = windowHeight / 10;
    client_struct->dimensions.menuButtonWidth = windowWidth / 3;

    return 0; // Success
}



int ticket_graphics(RequestPacket *request, Graphical_struct *client_struct)
{
    if (!request || !client_struct)
        return -1; // null pointer check

    int activeTextBox = -1;
    bool sent = false;

    // Temporary buffers
    char titleStr[128] = "";
    char descriptionStr[256] = "";
    char priorityStr[32] = "";

    // Length trackers
    int lenTitle = 0;
    int lenDesc = 0;
    int lenPriority = 0;

    while (!sent && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // base layout
        Vector2 startPos = {50, 50};
        int spacing = client_struct->dimensions.textBoxHeight + 20;

        // screen title
        draw_label("New Ticket:",
                   client_struct->font_pointer,
                   startPos,
                   client_struct->font_sizes.titleFontSize, BLACK);

        startPos.y += client_struct->font_sizes.titleFontSize + 35;

        // field definitions
        struct
        {
            const char *label;
            char *buffer;
            int maxLen;
            int *length;
        } fields[3] = {
            {"Title", titleStr, sizeof(titleStr) - 1, &lenTitle},
            {"Description", descriptionStr, sizeof(descriptionStr) - 1, &lenDesc},
            {"Priority", priorityStr, sizeof(priorityStr) - 1, &lenPriority}};

        // Draw each field
        for (int i = 0; i < 3; i++)
        {
            Vector2 pos = {startPos.x, startPos.y + i * spacing};

            Rectangle box;
            draw_text_box(fields[i].label,
                          fields[i].buffer,
                          pos,
                          activeTextBox == i,
                          client_struct,
                          &box);

            // catch mouse click to activate text box
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, box))
                    activeTextBox = i;
            }

            // handle keyboard input
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

        // send button
        Rectangle sendBtn = {
            startPos.x + client_struct->dimensions.labelWidth +
                client_struct->dimensions.textBoxWidth -
                client_struct->dimensions.formButtonWidth,
            startPos.y + spacing * 3,
            client_struct->dimensions.formButtonWidth,
            client_struct->dimensions.formButtonHeight};

        draw_button("SEND", client_struct, &sendBtn, client_struct->font_sizes.formButtonFontSize);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, sendBtn))
            {
                strcpy(request->data.new_ticket.title, (lenTitle > 0) ? titleStr : STR_UNUSED);
                strcpy(request->data.new_ticket.description, (lenDesc > 0) ? descriptionStr : STR_UNUSED);
                request->data.new_ticket.priority = (lenPriority > 0) ? atoi(priorityStr) : INT_UNUSED;

                sent = true;
            }
        }

        EndDrawing();
    }

    if (WindowShouldClose())
        return 1;

    return sent ? 0 : -1;
}


int req_type_graphics(RequestPacket *request, Graphical_struct *g_struct)
{
    if (!request || !g_struct)
        return -1; // null pointer check

    bool started = false;
    bool selected = false;

    while (!selected && !WindowShouldClose())
    {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Set the dimensions of the buttons based on screen size
        g_struct->font_sizes.menuButtonFontSize = screenH / 20;
        int titleFontSize = screenH / 15;

        BeginDrawing();
        ClearBackground(COLOR_BG);

        if (!started)
        {
            // first message to start the selection
            const char *startMsg = "Press SPACE to select request type";
            Vector2 textSize = MeasureTextEx(g_struct->font_pointer, startMsg,
                                             g_struct->font_sizes.menuButtonFontSize, 1);

            draw_label(startMsg,
                       g_struct->font_pointer,
                       (Vector2){(screenW - textSize.x) / 2,
                                 (screenH - textSize.y) / 2},
                       g_struct->font_sizes.menuButtonFontSize, BLACK);

            if (IsKeyPressed(KEY_SPACE))
                started = true;
        }
        else
        {
            // title of the selection screen
            const char *title = "Select Request Type";
            Vector2 titleSize = MeasureTextEx(g_struct->font_pointer, title, titleFontSize, 1);

            draw_label(title,
                       g_struct->font_pointer,
                       (Vector2){(screenW - titleSize.x) / 2, screenH / 10},
                       titleFontSize, BLACK);

            // vertical spacing between buttons
            int spacing = g_struct->dimensions.menuButtonHeight / 2;

            // buttons layout
            float centerX = (screenW - g_struct->dimensions.menuButtonWidth) / 2;
            float startY = screenH / 4;

            Rectangle buttons[4];
            buttons[0] = (Rectangle){centerX, startY,
                                     g_struct->dimensions.menuButtonWidth,
                                     g_struct->dimensions.menuButtonHeight};
            buttons[1] = (Rectangle){centerX, startY + (g_struct->dimensions.menuButtonHeight + spacing),
                                     g_struct->dimensions.menuButtonWidth,
                                     g_struct->dimensions.menuButtonHeight};
            buttons[2] = (Rectangle){centerX, startY + 2 * (g_struct->dimensions.menuButtonHeight + spacing),
                                     g_struct->dimensions.menuButtonWidth,
                                     g_struct->dimensions.menuButtonHeight};
            buttons[3] = (Rectangle){centerX, startY + 3 * (g_struct->dimensions.menuButtonHeight + spacing),
                                     g_struct->dimensions.menuButtonWidth,
                                     g_struct->dimensions.menuButtonHeight};

            const char *labels[4] = {"New ticket", "Query", "Sign in", "Query & Mod"};
            int types[4] = {REQ_CREATE_TICKET, REQ_QUERY, REQ_SIGNIN, REQ_QUERY_AND_MOD};

            // draw buttons
            for (int i = 0; i < 4; i++)
                draw_button(labels[i], g_struct, &buttons[i], g_struct->font_sizes.menuButtonFontSize);

            // click handling
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                for (int i = 0; i < 4; i++)
                {
                    if (CheckCollisionPointRec(mp, buttons[i]))
                    {
                        request->type = types[i];
                        selected = true;
                        break;
                    }
                }
            }
        }

        EndDrawing();
    }

    if (WindowShouldClose())
        return 1;

    return selected ? 0 : -1;
}


int filters_graphics(RequestPacket *request, bool agent_query_flag, Graphical_struct *g_struct)
{
    if (!request)
        return -1; // null pointer check

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
    int lenTitle = 0;
    int lenDesc = 0;

    while (!sent && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Layout parameters
        Vector2 startPos = {50, 50};
        int spacing = g_struct->dimensions.textBoxHeight + 20;

        // Title
        const char *title = "Query Filters:";
        draw_label(title,g_struct->font_pointer, startPos,g_struct->font_sizes.titleFontSize, BLACK);

        startPos.y += g_struct->font_sizes.titleFontSize + 35;

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

            Vector2 pos = {startPos.x, y};
            
            Rectangle box;
            draw_text_box(fields[i].label, fields[i].buffer, pos,
                          activeTextBox == i, g_struct, &box);



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
            startPos.x + g_struct->dimensions.labelWidth +
                g_struct->dimensions.textBoxWidth -
                g_struct->dimensions.formButtonWidth,
            startPos.y + spacing * 4,
            g_struct->dimensions.formButtonWidth,
            g_struct->dimensions.formButtonHeight};
            
        draw_button("SEND", g_struct, &sendBtn, g_struct->font_sizes.formButtonFontSize);

        // Check send click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, sendBtn))
            {
                // Chose which structure to compile
                if (agent_query_flag)
                {
                    request->data.Agent_query.filters.ticket_id = (lenTicket > 0) ? atoi(ticketIdStr) : INT_UNUSED;
                    request->data.Agent_query.filters.client_id = (lenClient > 0) ? atoi(clientIdStr) : INT_UNUSED;
                    strcpy(request->data.Agent_query.filters.title, (lenTitle > 0) ? titleStr : STR_UNUSED);
                    strcpy(request->data.Agent_query.filters.description, (lenDesc > 0) ? descriptionStr : STR_UNUSED);
                }
                else
                {
                    request->data.Client_query.ticket_id = (lenTicket > 0) ? atoi(ticketIdStr) : INT_UNUSED;
                    request->data.Client_query.client_id = (lenClient > 0) ? atoi(clientIdStr) : INT_UNUSED;
                    strcpy(request->data.Client_query.title, (lenTitle > 0) ? titleStr : STR_UNUSED);
                    strcpy(request->data.Client_query.description, (lenDesc > 0) ? descriptionStr : STR_UNUSED);
                }
                sent = true;
            }
        }

        EndDrawing();
    }

    // check if the window has been closed
    if (WindowShouldClose())
        return 1;

    return sent ? 0 : -1;
}

int modification_graphics(RequestPacket *request, Graphical_struct *g_struct)
{
    if (!request || !g_struct)
        return -1;

    int activeTextBox = -1;
    bool sent = false;

    char clientIdStr[64] = "";
    char priorityStr[64] = "";
    char passwordStr[64] = "";

    int lenClient = 0;
    int lenPriority = 0;
    int lenPassword = 0;

    int selectedStatus = -1;

    while (!sent && !WindowShouldClose())
    {
        int screenW = GetScreenWidth();

        BeginDrawing();
        ClearBackground(COLOR_BG);

        Vector2 startPos = {50, 50};
        int spacing = g_struct->dimensions.textBoxHeight + 20;

        // title
        const char *title = "Modify Ticket:";
        draw_label(title, g_struct->font_pointer, startPos, g_struct->font_sizes.titleFontSize, BLACK);

        // label over status
        const char *statusTitle = "New status";
        Vector2 statusTitleSize = MeasureTextEx(g_struct->font_pointer,
                                                statusTitle,
                                                g_struct->font_sizes.labelFontSize,
                                                1);

        int titleFontSize = g_struct->font_sizes.titleFontSize;
        draw_label(statusTitle,
                   g_struct->font_pointer,
                   (Vector2){startPos.x, startPos.y + titleFontSize + 20},
                   g_struct->font_sizes.labelFontSize, BLACK);

        // -statsus button
        const char *statusLabels[3] = {"Open", "Closed", "In Progress"};
        int numStatus = 3;

        int btnWidth = (screenW - 2 * startPos.x - 40) / numStatus;
        int btnHeight = g_struct->dimensions.formButtonHeight; 
        int btnY = startPos.y + titleFontSize + 20 + statusTitleSize.y + 15;

        for (int i = 0; i < numStatus; i++)
        {
            Rectangle btn = {
                startPos.x + i * (btnWidth + 10),
                btnY,
                btnWidth,
                btnHeight};

            // draw the buttons
            draw_button(statusLabels[i], g_struct, &btn, g_struct->font_sizes.formButtonFontSize);

            // handle the click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp = GetMousePosition();
                if (CheckCollisionPointRec(mp, btn))
                {
                    selectedStatus = i;
                }
            }

            // extra border for selected status
            if (selectedStatus == i)
            {
                DrawRectangleLinesEx(btn, 3, DARKBLUE); // draw a thicker border for the selected status
            }
        }

        // input field for client ID, priority, and password
        struct
        {
            const char *label;
            char *buffer;
            int maxLen;
            int *length;
        } fields[3] = {
            {"New Client ID", clientIdStr, sizeof(clientIdStr) - 1, &lenClient},
            {"New Priority", priorityStr, sizeof(priorityStr) - 1, &lenPriority},
            {"Client Password", passwordStr, sizeof(passwordStr) - 1, &lenPassword}};

        for (int i = 0; i < 3; i++)
        {
            Vector2 pos = {startPos.x, btnY + btnHeight + 30 + i * spacing};

            Rectangle box;
            draw_text_box(fields[i].label,
                          fields[i].buffer,
                          pos,
                          activeTextBox == i,
                          g_struct,
                          &box);

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

        // send button
        Rectangle sendBtn = {
            startPos.x + g_struct->dimensions.labelWidth +
                g_struct->dimensions.textBoxWidth -
                g_struct->dimensions.formButtonWidth,
            startPos.y + spacing * 5 + 3 * g_struct->dimensions.textBoxHeight,
            g_struct->dimensions.formButtonWidth,
            g_struct->dimensions.formButtonHeight};

        draw_button("SEND", g_struct, &sendBtn, g_struct->font_sizes.formButtonFontSize);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, sendBtn))
            {
                request->data.Agent_query.mod.new_status =
                    (selectedStatus >= 0) ? selectedStatus : INT_UNUSED;
                request->data.Agent_query.mod.new_client_id =
                    (lenClient > 0) ? atoi(clientIdStr) : INT_UNUSED;
                request->data.Agent_query.mod.new_priority =
                    (lenPriority > 0) ? atoi(priorityStr) : INT_UNUSED;
                request->data.Agent_query.pwd =
                    (lenPassword > 0) ? atoi(passwordStr) : INT_UNUSED;

                sent = true;
            }
        }

        EndDrawing();
    }

    if (WindowShouldClose())
        return 1;

    return sent ? 0 : -1;
}


int response_graphics(ResponsePacket *response, char *name, Graphical_struct *g_struct)
{
    char terminal_message[256];

    if (!response)
        return -1; // Null pointer check

    bool closed = false;
    bool buttonPressed = false;

    // show main message
    const char *mainMsg = NULL;
    Color mainColor;

    // write on the console what the server says
    strcpy(terminal_message, "Server says: ");
    strcat(terminal_message, response->message);

    mainColor = (Color){0, 255, 0, 255}; // green

    switch (response->type) // depend on what the request was
    {
    case RESP_QUERY_OK:
        mainMsg = "Succes";

        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_SUCCESS, "Server returned the information on the specified ticket", CLIENT, name);
        break;
    case RESP_AUTH_REQUIRED:
        mainMsg = "Error";
        mainColor = (Color){255, 0, 0, 255}; // red
        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_SUCCESS, "Authentication required wrong password", CLIENT, name);
        break;
    case RESP_SING_IN_OK:
        mainMsg = "Success";
        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_SUCCESS, "Client was sing-in succesfully", CLIENT, name);
        break;
    case RESP_TICKET_OK:
        mainMsg = "Success";
        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_SUCCESS, "Ticket added succesfully into the database", CLIENT, name);
        break;
    case RESP_QUERY_MOD_OK:
        mainMsg = "Success";
        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_SUCCESS, "Query and modification went fine", CLIENT, name);
        break;
    case RESP_ERROR:
        mainMsg = "Failed";
        mainColor = (Color){255, 0, 0, 255}; // red
        terminal_print(MSG_INFO, terminal_message, CLIENT, name);
        terminal_print(MSG_ERROR, "Query and modification failed", CLIENT, name);
        break;
    default:
        mainMsg = "";
        mainColor = COLOR_INPUT_TEXT;
        break;
    }

    // Get screen dimensions
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();


    Rectangle okButton = {
        (screenW - g_struct->dimensions.formButtonWidth) / 2,
        (screenH / 2) + (screenH / 6),
        g_struct->dimensions.formButtonWidth,
        g_struct->dimensions.formButtonHeight};

    // Main loop
    while (!closed && !buttonPressed && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Draw main message
        if (mainMsg && mainMsg[0] != '\0')
        {
            // Font size for main message
            Vector2 mainSize = MeasureTextEx(g_struct->font_pointer, mainMsg, g_struct->font_sizes.titleFontSize*2, 1);
            draw_label(mainMsg,
                       g_struct->font_pointer,
                       (Vector2){(screenW - mainSize.x) / 2, screenH / 4},
                       g_struct->font_sizes.titleFontSize*2, mainColor);

        }

        // Draw the message contained in the response
        if (response->message[0] != '\0')
        {
            Vector2 msgSize = MeasureTextEx(g_struct->font_pointer, response->message, g_struct->font_sizes.labelFontSize, 1);
            int yPos = (screenH / 2) + 10;
            draw_label(response->message,
                       g_struct->font_pointer,
                       (Vector2){(screenW - msgSize.x) / 2, yPos},
                       g_struct->font_sizes.labelFontSize, BLACK);
        }

        // Draw OK button 
        draw_button("OK", g_struct, &okButton, g_struct->font_sizes.formButtonFontSize);

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

    if (WindowShouldClose())
        return 1; // Window closed
    if (buttonPressed)
        return 0; // Button pressed

    return -1; // Something went wrong
}

int error_graphics( const char *errorMsg, Graphical_struct *g_struct)
{
    char path[256];

    bool buttonPressed = false;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Define title and colors
    const char *title = "Error";
    Color titleColor = (Color){255, 0, 0, 255}; // rosso


    // button rectangle
     Rectangle okButton = {
        (screenW - g_struct->dimensions.formButtonWidth) / 2,
        (screenH / 2) + (screenH / 6),
        g_struct->dimensions.formButtonWidth,
        g_struct->dimensions.formButtonHeight};

    // Main loop
    while (!buttonPressed && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Draw main title ("Errore")
        Vector2 titleSize = MeasureTextEx(g_struct->font_pointer, title, g_struct->font_sizes.titleFontSize * 2, 1);
        draw_label(title,
                   g_struct->font_pointer,
                   (Vector2){(screenW - titleSize.x) / 2, screenH / 4},
                   g_struct->font_sizes.titleFontSize * 2, titleColor);


        // Draw custom error message
        Vector2 msgSize = MeasureTextEx(g_struct->font_pointer, errorMsg, g_struct->font_sizes.labelFontSize, 1);
        int yPos = (screenH / 2) - (g_struct->font_sizes.labelFontSize / 2);
        draw_label(errorMsg,
                   g_struct->font_pointer,
                   (Vector2){(screenW - msgSize.x) / 2, yPos},
                   g_struct->font_sizes.labelFontSize, BLACK);

        // Draw OK button
        draw_button("OK", g_struct, &okButton, g_struct->font_sizes.formButtonFontSize);

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

    if (WindowShouldClose())
        return 1; // Window closed
    if (buttonPressed)
        return 0; // Button pressed

    return -1; // Something went wrong
}

void stop_client_window(Graphical_struct *client_struct, char *name)
{
    if (client_struct->font_pointer.texture.id != 0)
    {
        UnloadFont(client_struct->font_pointer);
        client_struct->font_pointer = (Font){0};
    }

    if (IsWindowReady()) // check if the window is open
    {
        CloseWindow();
    }

    terminal_print(MSG_INFO, "Client window closed", CLIENT, name);
}
