CC=gcc
CFLAGS=-Wall -Wextra -g
SANITIZE=-fsanitize=address
INCLUDES=-I./src -I./tests -I./tests/munit

# Find all .c files recursively
SRC := $(shell find src -type f -name "*.c")
# Exclude main.c from test build
SRC_NO_MAIN := $(filter-out src/core/main.c, $(SRC))
# Object files
OBJS := $(patsubst %.c,%.o,$(SRC_NO_MAIN))

# Test files
TEST_SRC := $(wildcard tests/test_vm.c tests/test_stack.c tests/test_lexer.c)
TEST_OBJ := $(TEST_SRC:.c=.o)

all: sneklang

# Compile sneklang binary with main.c
sneklang: $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o sneklang $(SRC)

# Run tests
test: test_runner
	./test_runner

# Build test runner
test_runner: tests/test_runner.c $(TEST_OBJ) $(OBJS) tests/munit/munit.c
	$(CC) $(CFLAGS) $(SANITIZE) $(INCLUDES) -o test_runner tests/test_runner.c $(TEST_OBJ) $(OBJS) tests/munit/munit.c

# Run sneklang with test scripts
run: sneklang
	./sneklang tests/scripts/test1.snek

# Clean up all object files & binaries
clean:
	rm -f sneklang test_runner
	find src tests -type f -name "*.o" -delete
