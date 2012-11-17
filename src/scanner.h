/**
 * Projekt: Implementace interpretu jazyka Falkon
 * Autori: Frantisek Kolacek
 *         Petr Matyas
 *         Dalibor Skacel
 *         Michaela Muzikarova
 *         Veronika Necasova
**/

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"
#include "strings.h"

extern FILE* gFileHandler;

// klicova slova
const char *klicovaSlova[9] = {
"else\0", "end\0", "false\0", "function\0", 
"if\0", "nil\0", "return\0", "true\0", "while\0"
};

  // rezervovana slova
const char *rezervovanaSlova[9] = {
"as\0", "def\0", "directove\0", "export\0", "from\0", 
"import\0", "launch\0", "load\0", "macro\0"
};

/**
 * stavy automatu
 * vyctovy typ
**/
typedef enum tstav{
  S_START,	//  0 -pocatecni stav
  S_KW,		//  1 - klicove slovo
  S_EOF,	//  2 - konec souboru
  S_ERR,	//  3 - chyba
  S_END,	//  4 - koncovy stav
  S_NUM,	//  5 - numeric
  S_STR,	//  6 - retezec
  S_BOOL,	//  7 - boolean
  S_PRIRAZENI,	//  8 - =
  S_TECKA,	//  9 - .
  S_LZ,		// 10 - (
  S_PZ,		// 11 - )
  S_STREDNIK, 	// 12 - ;
  S_CARKA,	// 13 - ,
  S_VYKRICNIK,	// 14 - !
  S_SOUCET,	// 15 - +
  S_ROZDIL,	// 16 - - 
  S_SOUCIN,	// 17 - *
  S_PODIL,	// 18 - /
  S_MOCNINA,	// 19 - **
  S_MENSI,	// 20 - <
  S_VETSI,       // 21 - >
  S_MENSIROVNO,	// 22 - <=
  S_VETSIROVNO,	// 23 - >=
  S_NEROVNASE,	// 24 - !=
  S_ROVNASE	// 25 - ==
} tStav;


/**
 * struktura tToken
 @stav - stav konecneho automatu
         vyctovy typ tStav
 @data - hodnota lexemu
         char *, retezec
**/   
typedef struct ttoken{
  tStav state;
  tStav nextState;
  
  unsigned int line;
  unsigned int column;
  
  tString data;
} tToken;

// globalni promenna token
extern tToken token;

void initToken(tToken*);
void pushToken(tToken*, int);
void getToken(tToken*);

#endif
