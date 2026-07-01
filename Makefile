# ==========================================
# Compiler Configuration
# ==========================================

# Compiler
CC = gcc
#CC = clang 

# Compiler flags:
# -Wall    : Enable common warnings
# -Wextra  : Enable additional warnings
# -std=c99 : Use the C99 language standard
# -O2      : Optimize generated code
# -Iinclude: Search for header files in ./include
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude

# Static library archiver
AR = ar
ARFLAGS = rcs

# ==========================================
# Library Source Files
# ==========================================

# Directory containing the library source files
SRC_DIR = src

# Library source files
SRCS = $(SRC_DIR)/gc.c $(SRC_DIR)/list.c

# Object files generated from the library sources
OBJS = $(SRCS:.c=.o)

# Name of the static library
LIB_NAME = libgc.a

# ==========================================
# User Program Configuration
# ==========================================

# Find every C source file in the current directory
USER_SRCS := $(wildcard *.c)

# Generate executable names by removing the .c extension
TARGETS := $(USER_SRCS:.c=)

# ==========================================
# Build Rules
# ==========================================

# Default target:
# Build the library and every user program.
all: $(TARGETS)

# Create the static library from the compiled object files.
$(LIB_NAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Compile each library source file into an object file.
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build any user program by linking it with the static library.
# Example:
#   demo.c -> demo
#   test.c -> test
%: %.c $(LIB_NAME)
	$(CC) $(CFLAGS) $< $(LIB_NAME) -o $@

# ==========================================
# Cleanup
# ==========================================

# Remove generated object files, the static library,
# and all executables built from user source files.
clean:
	rm -f $(OBJS) $(LIB_NAME) $(TARGETS)

# These targets do not correspond to real files.
.PHONY: all clean
