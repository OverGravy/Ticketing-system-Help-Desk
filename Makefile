CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -g

SRC_DIR := ./Src
OBJ_DIR := ./Obj
BIN_DIR := ./Bin

CLIENT_SRC_DIR := $(SRC_DIR)/Client
SERVER_SRC_DIR := $(SRC_DIR)/Server
COMMON_OBJ_DIR := $(OBJ_DIR)/Common
CLIENT_OBJ_DIR := $(OBJ_DIR)/Client
SERVER_OBJ_DIR := $(OBJ_DIR)/Server

# Tutti i file .c comuni (in Src/, esclusi i main)
COMMON_SOURCES := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.c')
COMMON_OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(COMMON_OBJ_DIR)/%.o, $(COMMON_SOURCES))

# File .c di Client (escludendo i main)
CLIENT_SOURCES := $(filter-out $(CLIENT_SRC_DIR)/Client_main.c $(CLIENT_SRC_DIR)/Agent_main.c, \
                    $(shell find $(CLIENT_SRC_DIR) -name '*.c'))
CLIENT_OBJECTS := $(patsubst $(CLIENT_SRC_DIR)/%.c, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SOURCES))

# File .c di Server (escludendo il main)
SERVER_SOURCES := $(filter-out $(SERVER_SRC_DIR)/Server_main.c, \
                    $(shell find $(SERVER_SRC_DIR) -name '*.c'))
SERVER_OBJECTS := $(patsubst $(SERVER_SRC_DIR)/%.c, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SOURCES))

# --- Executable Definitions ---

CLIENT_MAIN := $(CLIENT_SRC_DIR)/Client_main.c
CLIENT_MAIN_OBJ := $(CLIENT_OBJ_DIR)/Client_main.o
CLIENT_BIN := $(BIN_DIR)/Client

AGENT_MAIN := $(CLIENT_SRC_DIR)/Agent_main.c
AGENT_MAIN_OBJ := $(CLIENT_OBJ_DIR)/Agent_main.o
AGENT_BIN := $(BIN_DIR)/Agent

SERVER_MAIN := $(SERVER_SRC_DIR)/Server_main.c
SERVER_MAIN_OBJ := $(SERVER_OBJ_DIR)/Server_main.o
SERVER_BIN := $(BIN_DIR)/Server

BINARIES := $(CLIENT_BIN) $(AGENT_BIN) $(SERVER_BIN)

all: $(BINARIES)

# --- Binary Build Rules ---

$(CLIENT_BIN): $(CLIENT_MAIN_OBJ) $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(AGENT_BIN): $(AGENT_MAIN_OBJ) $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER_BIN): $(SERVER_MAIN_OBJ) $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# --- Compilation Rules ---

$(COMMON_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Clean ---

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
