/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: parser.h - Syntakticka analyza, generovani kodu
 * Autor: Kamil Dudka, xdudka00
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "tape.h"

#include <stdbool.h>
#include <stdio.h>

/*
 * Struktura obsahujici lokalni data Parseru - vyuziva modul expr
 */
typedef struct {
	const char *srcFileName;		// Ukazatel na jmeno zdrojoveho souboru
	PScannerData pScan;			// Ukazatel na Scanner zdrojoveho souboru
	TScannerToken token;			// Naposledy nacteny token
	bool bErrCompile;			// Chyba behem prekladu (negeneruje se kod)
	bool bSkipErr;				// Pøeskakuj chyby dokud tohle nikdo nevynuluje
	PInstTape pTape;			// Ukazatel na pasku (pseudokod)
} TParserData, *PParserData;

/*
 * Vstupni bod Parseru - odstartuje syntaxi rizeny preklad a interpretaci kodu
 * 1. parametr - ukazatel na otevreny zdrojovy soubor (moznost presmerovani)
 * 2. parametr - jmeno zdrojoveho souboru - muze byt obsazeno v chybovych hlaskach
 * vraci: kod chyby (nula znamena vsechno OK)
 */
int ParserInvoke (FILE *, const char *);

/*
 * Zapise pseudoinstrukci na vystupni pasku
 * 1. parametr - ukazatel na lokalni data parseru
 * 2. parametr - kod instrukce, ktera se ma vlozit
 * 3. parametr - ukazatel predany jako operand pseudoinstrukce
 */
void Generate (PParserData, TInstruction, void *);

/*
 * Obsluha syntakticke chyby
 *   - Vytiskne chybovou hlasku ve tvaru "file:line: msg\n"
 *   - zajisti zotaveni z chyby
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * 2. parametr+zbytek - stejne jako funkce printf ()
 */
void ParseError (PParserData, ...);

/*
 * Nacte dalsi token
 *   - vola funkci z modulu "scanner"
 *   - sam obslouzi lexikalni chybu
 * 1. parametr - ukazatel na lokalni data parseru
 */
int ParserGetNextToken (PParserData);
#endif
