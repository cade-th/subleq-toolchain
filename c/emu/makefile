CC = gcc
CFLAGS =-Isrc -g
LDFLAGS = 

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build
TEST_DIR = tests

# Find all .c files in src (excluding main.c for library/tests)
ALL_SRCS = $(shell find $(SRC_DIR) -name "*.c")
CORE_SRCS = $(filter-out $(SRC_DIR)/main.c, $(ALL_SRCS))
APP_SRCS = $(SRC_DIR)/main.c $(CORE_SRCS)

# Define object files
APP_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRCS))
CORE_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))

# Test files
UNIT_TEST_SRCS = $(filter-out $(TEST_DIR)/functional.c, $(shell find $(TEST_DIR) -name "*.c"))
UNIT_TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/tests/%.o, $(UNIT_TEST_SRCS))

APP_BIN = $(BIN_DIR)/6502_emu
UNIT_TEST_BIN = $(BIN_DIR)/unit_tests

all: $(APP_BIN)

# Link the main application
$(APP_BIN): $(APP_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Link the unit test suite
$(UNIT_TEST_BIN): $(UNIT_TEST_OBJS) $(CORE_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile src/ files to objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile tests/ files to objects
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(UNIT_TEST_BIN)
	./$(UNIT_TEST_BIN)

run: $(APP_BIN)
	./$(APP_BIN)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean run test 
