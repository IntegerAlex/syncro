# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -I/usr/include/openssl -g
LDFLAGS = -Llib -lssl -lcrypto -lz 

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
LIB_DIR = lib

# Source files and object files
SRCS = $(SRC_DIR)/add.c $(SRC_DIR)/dir.c $(SRC_DIR)/init.c $(SRC_DIR)/commit.c $(SRC_DIR)/main.c
OBJS = $(BUILD_DIR)/add.o $(BUILD_DIR)/dir.o $(BUILD_DIR)/init.o $(BUILD_DIR)/commit.o $(BUILD_DIR)/main.o

# Output binary
TARGET = ./build/syncro 

# Default target
all: $(TARGET)

# Rule to link object files into the final binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove compiled files
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

# Optional: Install target (if needed)
install:
	cp $(TARGET) /usr/local/bin/$(TARGET)

# Phony targets
.PHONY: all clean install

