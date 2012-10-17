# Projekt: IFJ05
# Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
#
# Makefile

BIN=ifj05
OBJ=ifj05.o scanner.o symbols.o variables.o parser.o expr.o tape.o runtime.o
ZIP=xdudka00.zip
EXPORT=Makefile dokumentace.pdf \
       ifj05.c scanner.c symbols.c variables.c parser.c expr.c tape.c runtime.c \
       ifj05.h scanner.h symbols.h variables.h parser.h expr.h tape.h runtime.h
TEST=test.txt
CC=gcc
DBGFLAGS?=-DNDEBUG
CFLAGS=-std=c99 -pedantic -Wall -W $(DBGFLAGS)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

ifj05.o: ifj05.c ifj05.h parser.h
	$(CC) $(CFLAGS) -c -o $@ ifj05.c

scanner.o: scanner.c ifj05.h scanner.h symbols.h variables.h
	$(CC) $(CFLAGS) -c -o $@ scanner.c

symbols.o: symbols.c ifj05.h symbols.h variables.h
	$(CC) $(CFLAGS) -c -o $@ symbols.c

variables.o: variables.c ifj05.h scanner.h variables.h
	$(CC) $(CFLAGS) -c -o $@ variables.c

parser.o: parser.c ifj05.h scanner.h symbols.h variables.h parser.h expr.h tape.h runtime.h 
	$(CC) $(CFLAGS) -c -o $@ parser.c

expr.o: expr.c ifj05.h scanner.h parser.h expr.h runtime.h
	$(CC) $(CFLAGS) -c -o $@ expr.c

tape.o: tape.c ifj05.h tape.h
	$(CC) $(CFLAGS) -c -o $@ tape.c

runtime.o: runtime.c ifj05.h scanner.h symbols.h variables.h tape.h runtime.h
	$(CC) $(CFLAGS) -c -o $@ runtime.c

check: $(BIN) $(TEST)
	valgrind --tool=memcheck ./$(BIN) $(TEST)

zip: $(ZIP)
$(ZIP): $(EXPORT)
	zip $(ZIP) $(EXPORT)

clean:
	rm -f $(BIN)
	rm -f $(OBJ)
