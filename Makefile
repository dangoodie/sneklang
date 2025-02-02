CC=gcc
CFLAGS=-Wall -Wextra -g

SRC=$(wildcard src/**/*.c)
TESTS=$(wildcard tests/*.c)

all: sneklang

sneklang: $(SRC)
	$(CC) $(CFLAGS) -o sneklang $(SRC)

test: $(TESTS)
	$(CC) $(CFLAGS) -o test_runner $(TESTS) -lmunit
	./test_runner

clean:
	rm -f sneklang test_runner
