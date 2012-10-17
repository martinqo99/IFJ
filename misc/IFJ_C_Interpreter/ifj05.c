/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: ifj05 - Hlavni modul prekladace obsahujici main()
 * Autor: Kamil Dudka, xdudka00
 */

#include "ifj05.h"
#include "parser.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static char szErrAlloc [] = "error: Out of memory!\n";
static void PrintHelp (void);

int main (int argc, char *argv[]) {
	if (argc <2) {
		// Malo parametru
		PrintHelp ();
		exit (1);
	}

	// Otevre zdrojovy soubor
	FILE *hFile = fopen (argv[1],"rt");
	if (NULL== hFile) {
		GlobalErr ("error: Could not open file!\n");
		exit (1);
	}

	// Spusti syntaxi rizeny preklad a interpretaci
	int iErr = ParserInvoke (hFile, argv[1]);

	// Uzavre soubor a vrati kod chyby
	fclose (hFile);
	return iErr;
}

/*
 * Tisk chybove hlasky
 * Parametry jsou stejne jako u funkce printf()
 */
void GlobalErr (const char *fmt, ...) {
	// Inicializace funkce s promennym poctem parametru
	va_list pArgs;
	va_start (pArgs, fmt);

	// Vypis chybove hlasky na stderr
	vfprintf (stderr, fmt, pArgs);

	// Ukoncovaci makro funkce s promennym poctem parametru
	va_end (pArgs);
}

/*
 * Tisk chybove hlasky
 * Parametry jsou stejne jako u funkce vprintf()
 */
void GlobalErrV (const char *fmt, va_list pArgs) {
	vfprintf (stderr, fmt, pArgs);
}

/*
 * Vola se pri neuspesne alokaci
 * Shodi prekladac
 */
void GlobalErrAlloc (void) {
	GlobalErr (szErrAlloc);
	exit (1);
}

static void PrintHelp (void) {
	printf ("Usage: ifj05 <file_name>\n");
}
