# Compiler
CC = gcc
CFLAGS = -save-temps -Wall -Werror -Wno-unused-variable -I./src

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRCS = $(shell find $(SRC_DIR) -name '*.c')

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executable
TARGET = taro

# Build rules
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)