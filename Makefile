all: test kilo

test: test.c
	$(CC) test.c -o test -Wall -Wextra -pedantic -std=c99

kilo: kilo.c
	$(CC) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99
