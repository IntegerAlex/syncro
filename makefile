# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -I/usr/include/openssl -g -std=c99 -D_POSIX_C_SOURCE=200112L
LDFLAGS = -Llib -lssl -lcrypto -lz

# Cross-compiler for Windows
# CC_WIN = x86_64-w64-mingw32-gcc
# CFLAGS_WIN = -Wall -Iinclude -I/usr/x86_64-w64-mingw32/include -g -std=c99 -D_POSIX_C_SOURCE=200112L
# LFLAGS_WIN = -Llib -lssl -lcrypto -lz -lws2_32  # Windows requires ws2_32 for networking

# Directories
SRC_DIR = src
BUILD_DIR_LINUX = build/linux
# BUILD_DIR_WIN = build/windows
INCLUDE_DIR = include
LIB_DIR = lib

# Source files
SRCS = add.c dir.c init.c hash.c commit.c main.c
OBJS_LINUX = $(SRCS:%.c=$(BUILD_DIR_LINUX)/%.o)
#OBJS_WIN = $(SRCS:%.c=$(BUILD_DIR_WIN)/%.o)

# Output binaries
TARGET_LINUX = $(BUILD_DIR_LINUX)/syncro.bin
#TARGET_WIN = $(BUILD_DIR_WIN)/syncro.exe

# Default target
all: $(BUILD_DIR_LINUX) $(BUILD_DIR_WIN) $(TARGET_LINUX) $(TARGET_WIN)

# Ensure build directories exist before compiling
$(BUILD_DIR_LINUX) $(BUILD_DIR_WIN):
	mkdir -p $(BUILD_DIR_LINUX) $(BUILD_DIR_WIN)

# Linux build rules
$(TARGET_LINUX): $(OBJS_LINUX)
	$(CC) $(OBJS_LINUX) -o $(TARGET_LINUX) $(LDFLAGS)

$(BUILD_DIR_LINUX)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR_LINUX)
	$(CC) $(CFLAGS) -c $< -o $@

# Windows build rules
#$(TARGET_WIN): $(OBJS_WIN)
#	$(CC_WIN) $(OBJS_WIN) -o $(TARGET_WIN) $(LDFLAGS_WIN)

#$(BUILD_DIR_WIN)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR_WIN)
#	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@

# Clean rule
clean:
	rm -rf build

# Run rule for Linux
run: $(TARGET_LINUX)
	$(TARGET_LINUX)
