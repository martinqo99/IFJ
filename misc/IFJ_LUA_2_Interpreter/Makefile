RM=rm
CC=gcc
CFLAGS=-c -Wall -std=c99 -O3 -pedantic

#timto se sestavi spustitelny soubor z *.o binarek
prekladac: main.o lexikalny_analyzator.o syntakticky_analyzator.o vyrazy.o ial.o funkcie.o interpret.o tagenerator.o advanced_strings.o vest_funkce.o clean.o garbage_collector.o
	$(CC) main.o lexikalny_analyzator.o syntakticky_analyzator.o vyrazy.o ial.o funkcie.o interpret.o tagenerator.o advanced_strings.o  vest_funkce.o clean.o garbage_collector.o -lm -o prekladac

clean:
	$(RM)   *.o
