CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -g

# Raylib linking flags (Linux)
RAYLIB_FLAGS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

SRC_DIR := ./Src
OBJ_DIR := ./Obj
BIN_DIR := ./Bin

CLIENTS_DIR := $(SRC_DIR)/Clients
CLIENT_SRC_DIR := $(CLIENTS_DIR)/Client
AGENT_SRC_DIR := $(CLIENTS_DIR)/Agent
SERVER_SRC_DIR := $(SRC_DIR)/Server
COMMON_OBJ_DIR := $(OBJ_DIR)/Common
CLIENT_OBJ_DIR := $(OBJ_DIR)/Client
AGENT_OBJ_DIR := $(OBJ_DIR)/Agent
SERVER_OBJ_DIR := $(OBJ_DIR)/Server

# Common source files inside Src (not main, not clients, not server)
COMMON_SOURCES := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.c')
COMMON_OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(COMMON_OBJ_DIR)/%.o, $(COMMON_SOURCES))

# Common client sources inside Src/Clients (shared by Client and Agent)
CLIENTS_COMMON_SOURCES := $(filter-out $(CLIENT_SRC_DIR)/*.c $(AGENT_SRC_DIR)/*.c, $(shell find $(CLIENTS_DIR) -maxdepth 1 -name '*.c'))
CLIENTS_COMMON_OBJECTS := $(patsubst $(CLIENTS_DIR)/%.c, $(COMMON_OBJ_DIR)/Clients/%.o, $(CLIENTS_COMMON_SOURCES))

# Client-specific sources excluding main
CLIENT_SOURCES := $(filter-out $(CLIENT_SRC_DIR)/Client_main.c, $(shell find $(CLIENT_SRC_DIR) -name '*.c'))
CLIENT_OBJECTS := $(patsubst $(CLIENT_SRC_DIR)/%.c, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SOURCES))

# Agent-specific sources excluding main
AGENT_SOURCES := $(filter-out $(AGENT_SRC_DIR)/Agent_main.c, $(shell find $(AGENT_SRC_DIR) -name '*.c'))
AGENT_OBJECTS := $(patsubst $(AGENT_SRC_DIR)/%.c, $(AGENT_OBJ_DIR)/%.o, $(AGENT_SOURCES))

# Server-specific sources excluding main
SERVER_SOURCES := $(filter-out $(SERVER_SRC_DIR)/Server_main.c, $(shell find $(SERVER_SRC_DIR) -name '*.c'))
SERVER_OBJECTS := $(patsubst $(SERVER_SRC_DIR)/%.c, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SOURCES))

# Main files and objects
CLIENT_MAIN := $(CLIENT_SRC_DIR)/Client_main.c
CLIENT_MAIN_OBJ := $(CLIENT_OBJ_DIR)/Client_main.o
CLIENT_BIN := $(BIN_DIR)/Client

AGENT_MAIN := $(AGENT_SRC_DIR)/Agent_main.c
AGENT_MAIN_OBJ := $(AGENT_OBJ_DIR)/Agent_main.o
AGENT_BIN := $(BIN_DIR)/Agent

SERVER_MAIN := $(SERVER_SRC_DIR)/Server_main.c
SERVER_MAIN_OBJ := $(SERVER_OBJ_DIR)/Server_main.o
SERVER_BIN := $(BIN_DIR)/Server

BINARIES := $(CLIENT_BIN) $(AGENT_BIN) $(SERVER_BIN)

all: $(BINARIES)

# Build Client binary (with Raylib)
$(CLIENT_BIN): $(CLIENT_MAIN_OBJ) $(CLIENT_OBJECTS) $(CLIENTS_COMMON_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(RAYLIB_FLAGS)

# Build Agent binary (with Raylib)
$(AGENT_BIN): $(AGENT_MAIN_OBJ) $(AGENT_OBJECTS) $(CLIENTS_COMMON_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(RAYLIB_FLAGS)

# Build Server binary (with sqlite3, without Raylib)
$(SERVER_BIN): $(SERVER_MAIN_OBJ) $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -lsqlite3 -o $@

# Compile common objects in Src/
$(COMMON_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile common Clients objects (e.g. common_net_com.c)
$(COMMON_OBJ_DIR)/Clients/%.o: $(CLIENTS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Client objects
$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Agent objects
$(AGENT_OBJ_DIR)/%.o: $(AGENT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Server objects
$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

