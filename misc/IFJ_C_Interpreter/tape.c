/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: tape.h - modul pasky instrukci
 * Autor: David Barina, xbarin02
 */

#include <stdlib.h>

#include "ifj05.h"
#include "tape.h"

/***
 * ErrorFatal()
 *   moje pomocna funkce pro nahlaseni kriticke chyby a ukonceni programu
 *   vyuziva globalni funkci pro hlaseni chyb GlobalErr()
 *   parametry stejne jako funkce GlobalErr resp. printf()
 */
extern void ErrorFatal(const char *fmt, ...);

/***
 * TapeCreate
 *   vytvori prazdnou pasku a vrati ukazatel na ni
 *   parametr - pocet instrukci pro pridani pri zvetsovani pasky
 */
PInstTape TapeCreate(unsigned append_size)
{
  PInstTape tape = (PInstTape)malloc(sizeof(TInstTape));
  if(tape == NULL)
    ErrorFatal("Not enough of memory");
  tape->insts = NULL;
  tape->count = 0;
  tape->allocated = 0;
  tape->append = append_size;
  return(tape);
}

/***
 * TapeAddInst()
 *   prida instrukci do pasky (zkopiruje ji!)
 *   1. parametr - adresa pasky
 *   2. parametr - adresa instrukce (neprovede se na ni free)  
 */
void TapeAddInst(PInstTape tape, PInstBlock instruction)
{
#ifndef NDEBUG
  if(tape == NULL)
    ErrorFatal("Internal error - inserting to NULL tape");
  if(instruction == NULL)
    ErrorFatal("Internal error - inserting NULL instruction");
#endif
  if(tape->count >= tape->allocated)
  {
    tape->insts = realloc(tape->insts,
                          sizeof(TInstBlock) *
                          (tape->allocated + tape->append) );
    if(tape->insts == NULL)
      ErrorFatal("Not enought of memory for reallocation");
    tape->allocated += tape->append;
    tape->append <<= 1;
  }
  // struktura se zkopiruje
  tape->insts[tape->count++] = *instruction;
}

/***
 * TapeGetNextIndex()
 *   vrati index dalsi instrukce v pasce (pro label)
 *   parametr - adresa pasky 
 */
unsigned TapeGetNextIndex(PInstTape tape)
{
#ifndef NDEBUG
  if(tape == NULL)
    ErrorFatal("Internal error - NULL tape");
#endif
  return(tape->count);
}

/***
 * TapeDestroy
 *   uvolni pasku z pameti
 *   parametr - adresa pasky  
 */
void TapeDestroy(PInstTape tape)
{
#ifndef NDEBUG
  if(tape == NULL)
    ErrorFatal("Internal error - NULL tape");
#endif
  free(tape->insts);
  free(tape);
}
