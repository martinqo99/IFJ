PRJ=ifj12
#
PROGS=$(PRJ)
FILES=main.c mmu.c symbol_table.c strings.c scanner.c
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ): $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f *.o *.out $(PROGS)
