/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   ial.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#pragma once

#include <stdbool.h>

/**
 * Datove struktury - Binarny vyhladavaci strom
 */
typedef char *tNazov;   // kluc pre binarne vyhladavanie

typedef enum            // enumeracia pre interne typy symbolov
{
    tNil,               // null
    tBool,
    tDouble,
    tString,
} tTypSymbolu;

typedef union               // unia pre data symbolu
{
    bool b;
    double d;
    char *s;
} tObsah;

typedef struct   // symbol
{
    tNazov nazov;       // nazov symbolu
    tTypSymbolu typ;    // datovy typ symbolu
    bool varFc;         // true je funkce a false je var
    int argCounter;
    void * nextNode; //prechod na dalsi argument funkce
    tObsah value;
    int jump;
} tSymbol, *tSymbolPtr;

typedef struct tBTSUzol
{
    tNazov kluc;
    tSymbol data;
    struct tBTSUzol *lptr;
    struct tBTSUzol *rptr;
} *tBTSUzolPtr;	        // uzol binarneho stromu

/**
 * Globalne premenne
 */
extern tBTSUzolPtr ts;  // tabulka symbolov

/**
 * Prototypy funkcii
 */
void TSinit(void);                              // inicializuje tabulku symbolov
void TSdispose(void);                           // zrusi tabulku symbolov
void TSinitSymbol(tSymbol *symbol);             // inicializuje symbol pri deklaracii
tSymbol *TSvlozSymbol(tSymbol data);              // vlozi symbol do tabulky
tSymbol *TSvlozBool(tNazov nazov, bool data);
tSymbol *TSvlozDouble(tNazov nazov, double data);
tSymbol *TSvlozString(tNazov nazov, char *string);
tBTSUzolPtr TSreadSymbol(tNazov nazov);              // precita symbol z tabulky
void ialSort(char *string);                     // funkcia potrebna pre nas
int ialSearch(char *string, char *pattern);     // nasa funkcia

tBTSUzolPtr BTSsearch(tBTSUzolPtr koren, tNazov kluc);


void quickSort(char* stringToSort, int left, int right);
tBTSUzolPtr prep_quickSort(tBTSUzolPtr input);
void calcCharJumps(int charJump[], char* stringToFind, int len_stringToFind);
void calcMatchJumps(int matchJump[], char *patCont, int m);
tBTSUzolPtr find(tBTSUzolPtr input, tBTSUzolPtr inputToFind);