#ifndef __TERMINAL__
#define __TERMINAL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// COLORS

// Process colors
#define SERVER_COLOR       "\033[1;34m"  // Bold Blue
#define AGENT_STD_COLOR    "\033[1;32m"  // Bold Green
#define AGENT_SPEC_COLOR   "\033[1;35m"  // Bold Magenta

// Message type colors
#define MSG_INFO_COLOR     "\033[1;37m"  // Bold White
#define MSG_SUCCESS_COLOR  "\033[1;92m"  // Bright Green
#define MSG_ERROR_COLOR    "\033[1;91m"  // Bright Red

// Reset
#define ANSI_RESET         "\033[0m"

typedef enum {
    SERVER,
    CLIENT,
    AGENT
} Role;

typedef enum {
    MSG_INFO,
    MSG_SUCCESS,
    MSG_ERROR
} MessageType;


// function tha print different type of message on the terminal
void terminal_print(MessageType type, const char* message, Role role, const char* name);

#endif