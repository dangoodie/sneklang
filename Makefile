CC=gcc
CFLAGS=-Wall -Wextra -g
SANITIZE=-fsanitize=address
INCLUDES=-I./src -I./tests -I./tests/munit

SRC=$(wildcard src/**/*.c)
SRC := $(filter-out src/core/main.c, $(SRC))  # Exclude main.c from test build
TEST_SRC=$(wildcard tests/test_vm.c tests/test_stack.c tests/test_lexer.c)
TEST_OBJ=$(TEST_SRC:.c=.o)
OBJS=$(SRC:.c=.o)

all: sneklang

sneklang: $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o sneklang $(SRC)

test: test_runner
	./test_runner

test_runner: tests/test_runner.c $(TEST_OBJ) $(OBJS) tests/munit/munit.c
	$(CC) $(CFLAGS) $(SANITIZE) $(INCLUDES) -o test_runner tests/test_runner.c $(TEST_OBJ) $(OBJS) tests/munit/munit.c

clean:
	rm -f sneklang test_runner *.o
