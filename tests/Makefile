PRJ=strings
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c
	$(CC) $(CFLAGS) -o $@ $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c

clean:
	rm -f *.o *.out $(PROGS)
