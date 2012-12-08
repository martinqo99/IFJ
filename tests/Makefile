PRJ=syntactic
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c ../src/strings.c ../src/errors.c ../src/list.c ../src/binary_tree.c ../src/scanner.c ../src/stack.c ../src/expression.c ../src/symbol_table.c
	$(CC) $(CFLAGS) -o $@ $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c ../src/strings.c ../src/errors.c ../src/list.c ../src/binary_tree.c ../src/scanner.c ../src/stack.c ../src/expression.c ../src/symbol_table.c

clean:
	rm -f *.o *.out $(PROGS)
