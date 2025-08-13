#include "../../../Libs/Clients/Client/Client_graphics.h"

int init_client_window(int id)
{
    // Step 1: create a small initial window to avoid GLFW warnings
    const int initialWidth = 200;
    const int initialHeight = 200;

    char title[64];
    if (snprintf(title, sizeof(title), "Client %d", id) < 0)
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
