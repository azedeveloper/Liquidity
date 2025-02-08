#!/bin/bash

# Find all .c files in the current directory
SRC_FILES=$(ls *.c 2>/dev/null)

# Check if any .c files exist
if [ -z "$SRC_FILES" ]; then
    echo "No .c files found in the current directory."
    exit 1
fi

# Compile with GCC and SDL2
gcc $SRC_FILES -o app $(sdl2-config --cflags --libs) -Iinclude/ -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created: ./app"
else
    echo "Build failed."
fi
