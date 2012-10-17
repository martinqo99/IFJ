CFLAGS=-std=c99 -Wall -pedantic -g -O3
BIN=project
CC=gcc
RM=rm -r
MODULS= str.o list.o stack.o table.o parser.o scanner.o expression.o interpret.o binary_tree.o library.o main.o

ALL: $(MODULS)
	$(CC) $(FLAGS) -o $(BIN) $(MODULS) -lm

clean: 
	$(RM) *.o $(BIN)


