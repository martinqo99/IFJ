PRJ=ifj12
#
PROGS=$(PRJ)
FILES=main.c mmu.c hash_table.c strings.c scanner.c kmp.c syntactic.c
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ): $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f *.o *.out $(PROGS)
