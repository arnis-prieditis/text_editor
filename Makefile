CARGS= -Wall -Wextra -pedantic -std=c99
all: kilo

kilo: kilo.c
	$(CC) kilo.c -o kilo $(CARGS)
clean:
	$(RM) kilo
