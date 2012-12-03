PRJ=scanner
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c ../src/strings.c ../src/errors.c ../src/list.c ../src/binary_tree.c
	$(CC) $(CFLAGS) -o $@ $(PRJ)-test.c ../src/$(PRJ).c ../src/mmu.c ../src/strings.c ../src/errors.c ../src/list.c ../src/binary_tree.c

clean:
	rm -f *.o *.out $(PROGS)
