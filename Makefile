CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -g

SRC_DIR := ./Src
OBJ_DIR := ./Obj
BIN_DIR := ./Bin

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = TiketingSystem
BIN_TARGET = $(BIN_DIR)/$(TARGET)

all: $(BIN_TARGET)

$(BIN_TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN_TARGET) $(LDFLAGS) -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(BIN_TARGET)

.PHONY: all clean