PRJ=ifj12
#
PROGS=$(PRJ)
FILES=main.c errors.c mmu.c symbol_table.c strings.c scanner.c kmp.c syntactic.c stack.c expression.c library.c quicksort.c list.c binary_tree.c interpret.c
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic

all: $(PROGS)

$(PRJ): $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES) -lm

clean:
	rm -f *.o *.out $(PROGS)
