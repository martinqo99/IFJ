/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: symbols.h - modul tabulky symbolu
 * Autor: David Barina, xbarin02
 */

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "variables.h"

/***
 * TSymbolType
 *   kazdy symbol muze byt jednim z techto typu 
 */
typedef enum
        {
          stVariable,   // na zacatku je kazdy identifikator promena
          stLabel       // o nekterych se dozvime, ze jsou to labely
        } TSymbolType;

/***
 * TSymbols
 *   vlastni struktura symbolu
 */
typedef struct _TSymbol
        TSymbol, *PSymbol;

/***
 * TSymbols
 *   pozor na jmeno prechoziho typu, struktura zastresujici tabulku symbolu
 */ 
typedef struct _TSymbols
        TSymbols, *PSymbols;

/***
 * SymbolsCreate()
 *   vytvori prazdnou tabulku symbolu a vrati ukazatel na ni
 */ 
PSymbols SymbolsCreate();

/***
 * SymbolsInsert()
 *   vlozi symbol do tabulky; jestlize existuje, vrati jen
 *   ukazatel na nej, jinak provede vlozeni a take vrati ukazatel na nej;
 *   parametr - nazev symbolu 
 */ 
PSymbol SymbolsInsert(PSymbols table, char *strName, PVariableTable vartable);

/***
 * SymbolsSetLabel()
 *   udela z identifikatoru label
 *   1. parametr - adresa symbolu vracena funkci SymbolsInsert()
 *   2. parametr - index do pasky, kam label ukazuje
 *   ! pri pokusu o redefinici labelu vraci nenulovou hodnotu, jinak 0
 */
int SymbolsSetLabel(PSymbol symbol, unsigned index);

/***
 * SymbolsGetType()
 *   funkce pro runtime, kt. vrati typ symbolu
 *   parametr - adresa symbolu 
 */
TSymbolType SymbolsGetType(PSymbol symbol);

/***
 * SymbolsGetName()
 *   funkce pro runtime, kt. vrati jmeno symbolu (pro vypis chyb)
 *   parametr - adresa symbolu
 */
char *SymbolsGetName(PSymbol symbol);

/***
 * SymbolsGetLabelIndex()
 *   funkce pro runtime, kt. vrati index do pasky u labelu
 *   parametr - adresa symbolu  
 */
unsigned SymbolsGetLabelIndex(PSymbol symbol);

/***
 * SymbolsGetVariable()
 *   funkce pro runtime, kt. vrati odkaz na promenou 
 *   parametr - adresa symbolu 
 */
PVariable SymbolsGetVariable(PSymbol symbol);

/***
 * SymbolsDestroy()
 *   uvolni vsechny symboly + tabulku symbolu z pameti
 *   parametr - adresa tabulky symbolu  
 */ 
void SymbolsDestroy(PSymbols table);

#endif
