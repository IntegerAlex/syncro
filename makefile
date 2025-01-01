# Variables
CC = gcc
CFLAGS = -Iinclude -Wall
SRC = src/dir.c src/init.c src/main.c
OBJ = build/dir.o build/init.o build/main.o
TARGET = build/vcs

# Default target
all: $(TARGET)

# Linking object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compiling source files into object files
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f build/*.o $(TARGET)

