#include "../Libs/Terminal_com.h"

void terminal_print(MessageType type, const char *message, Role role, const char *name)
{

    const char *name_color;
    const char *msg_color;

    switch (type)
    {
    case MSG_INFO:
        msg_color = MSG_INFO_COLOR;
        break;
    case MSG_SUCCESS:
        msg_color = MSG_SUCCESS_COLOR;
        break;
    case MSG_ERROR:
        msg_color = MSG_ERROR_COLOR;
        break;
    default:
        msg_color = ANSI_RESET;
        break;
    }

    switch (role)
    {
    case SERVER:
        name_color = SERVER_COLOR;
        break;
    case CLIENT:
        name_color = AGENT_STD_COLOR;
        break;
    default:
        name_color = ANSI_RESET;
        break;
    }
    printf("%s[%s]%s: %s%s%s\n",
           name_color, name, ANSI_RESET,
           msg_color, message, ANSI_RESET);
    fflush(stdout); // Ensure the message is printed immediately
}
