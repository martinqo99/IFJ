PRJ=ifj12
#
PROGS=$(PRJ)
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ): main.c mmu.c hash_table.c strings.c scanner.c
	$(CC) $(CFLAGS) -o $@ main.c mmu.c hash_table.c strings.c scanner.c

clean:
	rm -f *.o *.out $(PROGS)
