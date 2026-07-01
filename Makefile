# ==========================================
# Compiler Configurations
# ==========================================

#CC = clang
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude
AR = ar
ARFLAGS = rcs

# ==========================================
# File Targets
# ==========================================

# Internal Library files
SRC_DIR = src
SRCS = $(SRC_DIR)/gc.c $(SRC_DIR)/list.c
OBJS = $(SRCS:.c=.o)
LIB_NAME = libgc.a

# User Space configuration
USER_SRCS := $(wildcard *.c)
TARGETS := $(USER_SRCS:.c=)

# ==========================================
# Build Rules
# ==========================================

# Default target: builds the library and compiles the user's programs
all: $(TARGETS)

# links user's main.c with compiled static library
$(TARGET): $(USER_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) $(USER_SRC) $(LIB_NAME) -o $(TARGET)

# packs compiled object files into a reusable static library file
$(LIB_NAME): $(OBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(OBJS)

# compiles raw C files into object (.o) files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# finds any C files and compiles them with compiled static library 
%: %.c $(LIB_NAME)
	$(CC) $(CFLAGS) $< $(LIB_NAME) -o $@

#Clean up builds
clean:
	rm -f $(SRC_DIR)/*.o $(LIB_NAME) $(TARGET)

.PHONY: all clean
