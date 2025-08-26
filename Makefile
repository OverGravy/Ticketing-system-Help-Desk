# compiler flags
CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -g

# library flags
RAYLIB_FLAGS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11
SQLITE_FLAGS = -lsqlite3

# Directory
SRC_DIR   := ./Src
OBJ_DIR   := ./Obj
BIN_DIR   := ./Bin

CLIENT_DIR := $(SRC_DIR)/Client
SERVER_DIR := $(SRC_DIR)/Server
COMMON_OBJ_DIR := $(OBJ_DIR)/Common
CLIENT_OBJ_DIR := $(OBJ_DIR)/Client
SERVER_OBJ_DIR := $(OBJ_DIR)/Server

# common sources and objects
COMMON_SOURCES := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.c')
COMMON_OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(COMMON_OBJ_DIR)/%.o, $(COMMON_SOURCES))


# client 
CLIENT_MAIN     := $(CLIENT_DIR)/Client_main.c
CLIENT_MAIN_OBJ := $(CLIENT_OBJ_DIR)/Client_main.o
CLIENT_SOURCES  := $(filter-out $(CLIENT_MAIN), $(shell find $(CLIENT_DIR) -name '*.c'))
CLIENT_OBJECTS  := $(patsubst $(CLIENT_DIR)/%.c, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SOURCES))
CLIENT_BIN      := $(BIN_DIR)/Client

# server
SERVER_MAIN     := $(SERVER_DIR)/Server_main.c
SERVER_MAIN_OBJ := $(SERVER_OBJ_DIR)/Server_main.o
SERVER_SOURCES  := $(filter-out $(SERVER_MAIN), $(shell find $(SERVER_DIR) -name '*.c'))
SERVER_OBJECTS  := $(patsubst $(SERVER_DIR)/%.c, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SOURCES))
SERVER_BIN      := $(BIN_DIR)/Server

# final binaries
BINARIES := $(CLIENT_BIN) $(SERVER_BIN)

all: $(BINARIES)

# --- Link ---
$(CLIENT_BIN): $(CLIENT_MAIN_OBJ) $(CLIENT_OBJECTS) $(CLIENTS_COMMON_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(RAYLIB_FLAGS)

$(SERVER_BIN): $(SERVER_MAIN_OBJ) $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(SQLITE_FLAGS)

# compile
$(COMMON_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(COMMON_OBJ_DIR)/Clients/%.o: $(SRC_DIR)/Clients/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
