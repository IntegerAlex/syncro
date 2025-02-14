# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -I/usr/include/openssl -g -std=c99 -D_POSIX_C_SOURCE=200112L
LDFLAGS = -Llib -lssl -lcrypto -lz

# Directories
SRC_DIR = src
BUILD_DIR_LINUX = build/linux
# BUILD_DIR_WIN = build/windows
INCLUDE_DIR = include
LIB_DIR = lib

# Source files
SRCS = diff.c inflate.c add.c rm.c dir.c init.c hash.c commit.c main.c
OBJS_LINUX = $(SRCS:%.c=$(BUILD_DIR_LINUX)/%.o)

# Output binaries
TARGET_LINUX = $(BUILD_DIR_LINUX)/syncro.bin

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

# Clean rule
clean:
	rm -rf build

# Run rule for Linux
run: $(TARGET_LINUX)
	$(TARGET_LINUX)

# Install rule for Linux
install: $(TARGET_LINUX)
	# Copy binary to /usr/local/bin
	sudo cp $(TARGET_LINUX) /usr/local/bin/syncro
	# Ensure it's executable
	sudo chmod +x /usr/local/bin/syncro
	# Optionally, you can also add a symlink to /usr/bin if needed
	# sudo ln -s /usr/local/bin/syncro /usr/bin/syncro

	@echo "syncro installed successfully. You can now run it with 'syncro' from anywhere."

# Uninstall rule (optional)
uninstall:
	sudo rm -f /usr/local/bin/syncro
	@echo "syncro uninstalled successfully."
