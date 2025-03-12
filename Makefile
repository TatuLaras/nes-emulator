BUILD_DIR = build
BUILD_DIR_TESTS = build/tests
SRC_DIR = src
SRC_DIR_TESTS = test
UNITY_DIR = external/unity
MKDIR = mkdir

CC = gcc
CFLAGS_DEBUG = -Wall -ggdb -DDEBUG
CFLAGS_TEST= -Wall -ggdb -I$(UNITY_DIR) -I$(SRC_DIR) -DTEST
CFLAGS= -Wall

# Arguments to append to the program run with "make run"
APPEND_ARGS = 

# Build program

SRC = $(wildcard $(SRC_DIR)/*.c)


debug: $(BUILD_DIR) $(BUILD_DIR)/debug
release: $(BUILD_DIR) $(BUILD_DIR)/release

run: $(BUILD_DIR) $(BUILD_DIR)/debug
	$(BUILD_DIR)/debug $(APPEND_ARGS)

$(BUILD_DIR)/debug: $(SRC)
	@echo "Building debug build"
	$(CC) -o $@ $^ $(CFLAGS_DEBUG)

$(BUILD_DIR)/release: $(SRC)
	@echo "Building release build"
	$(CC) -o $@ $^ $(CFLAGS)

$(BUILD_DIR):
	$(MKDIR) -p $(BUILD_DIR)

$(BUILD_DIR_TESTS):
	$(MKDIR) -p $(BUILD_DIR_TESTS)


# Build and run tests

SRC_FOR_TESTS = $(filter-out $(SRC_DIR)/main.c, $(SRC)) $(wildcard $(UNITY_DIR)/*.c)
OBJS_TESTS = $(patsubst $(SRC_DIR_TESTS)/%.c, $(BUILD_DIR_TESTS)/%.o, $(wildcard $(SRC_DIR_TESTS)/test_*.c))

test: $(BUILD_DIR_TESTS) run_tests
	@echo

NOOP=
SPACE = $(NOOP) $(NOOP)

run_tests: $(OBJS_TESTS)
	@echo -e "\n\n--------------\n Test results\n--------------\n"
	@$(subst $(SPACE), && echo -e "\n" && ,$^)

$(OBJS_TESTS): $(BUILD_DIR_TESTS)/%.o: $(SRC_DIR_TESTS)/%.c $(SRC_FOR_TESTS)
	@echo -e "\nBuilding $@"
	$(CC) -o $@ $^ $(CFLAGS_TEST)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BUILD_DIR_TESTS)

