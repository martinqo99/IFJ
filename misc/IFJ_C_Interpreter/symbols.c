/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: symbols.c - modul tabulky symbolu
 * Autor: David Barina, xbarin02
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ifj05.h"
#include "symbols.h"
#include "variables.h"

/***
 * TSymbolAddr
 *   kazdy symbol ma nejakou hodnotu, u labelu je ji index do pasky,
 *   u promenych adresa bunky v tabulce promennych (run-time)  
 */
typedef union
        {
          unsigned LabelIndex;    // tohle je platny pro typ stLabel
          PVariable VariablePtr;  // a tohle pro stVariable
        } TSymbolAddr;

struct _TSymbol
       {
         char *name;         // jmeno symbolu je zaroven klic
         TSymbolType type;   // typ symbolu (label/variable)
         TSymbolAddr addr;   // obsah (index labelu/adresa bunky promene)
         PSymbol LPtr, RPtr; // binarni strom
       };

/***
 * struct _TSymbols
 *   reprezentace tabulky symbolu;
 *   obsahuje pouze ukazatel na prvni prvek binarniho vyhledavaciho stromu
 */ 
struct _TSymbols
       {
         PSymbol root;
       };

/***
 * ErrorFatal()
 *   moje pomocna funkce pro nahlaseni kriticke chyby a ukonceni programu
 *   vyuziva globalni funkci pro hlaseni chyb GlobalErr()
 *   parametry stejne jako funkce GlobalErr resp. printf()
 */
extern void ErrorFatal(const char *fmt, ...);

/***
 * SymbolsCreate()
 *   vytvori prazdnou tabulku symbolu a vrati ukazatel na ni 
 */ 
PSymbols SymbolsCreate()
{
  PSymbols table = (PSymbols)malloc(sizeof(TSymbols));
  if(table == NULL)
    ErrorFatal("Not enough of memory");
  table->root = NULL;
  return(table);
}

/***
 * SymbolInsert()
 *   rekurzivni funkce vkladani; pomocna funkce SymbolsInsert()
 *   vraci ukazatel na vlozeny (existujici) symbol
 *   1. parametr - adresa ukazatele na soucasny prvek
 *   2. parametr - adresa jmena symbolu 
 */ 
PSymbol SymbolInsert(PSymbol *parent, char *name, PVariableTable vartable)
{
#ifndef NDEBUG
  if(parent == NULL)
    ErrorFatal("Internal error - NULL symbol pointer");
  if(name == NULL)
    ErrorFatal("Internal error - no new symbol name");
#endif
  // zjisteni existence prvku
  if(*parent == NULL)
  {
    // mame ho, sem budeme vkladat
    PSymbol symbol = (PSymbol)malloc(sizeof(TSymbol));
    if(symbol == NULL)
      ErrorFatal("Not enough of memory");
    symbol->name = name;
    // na pocatku je kazdy identifikator promena
    symbol->type = stVariable;
    // musi se vytvorit zaznam v tabulce promenych!!!!!!
    symbol->addr.VariablePtr = VariablesInsert(vartable);
    symbol->LPtr = symbol->RPtr = NULL;
    *parent = symbol;
    return(symbol);
  }
  else
  {
    // uz je tam?
    if(0 == strcmp((*parent)->name,name))
    {
      free(name);
      return(*parent);
    }
    else
    {
      // je vlevo ci vpravo?
      if(0 < strcmp((*parent)->name,name))
        return( SymbolInsert( &((*parent)->RPtr), name, vartable) );
      else
        return( SymbolInsert( &((*parent)->LPtr), name, vartable) );
    }
  }
}

/***
 * SymbolsInsert()
 *   vlozi symbol do tabulky; jestlize existuje, vrati jen
 *   ukazatel na nej, jinak provede vlozeni a take vrati ukazatel na nej;
 *   parametr - nazev symbolu 
 */ 
PSymbol SymbolsInsert(PSymbols table, char *strName, PVariableTable vartable)
{
#ifndef NDEBUG
  if(strName == NULL)
    ErrorFatal("Internal error - no symbol name");
  if(table == NULL)
    ErrorFatal("Internal error - no symbol table");
#endif
  return(SymbolInsert(&(table->root),strName,vartable));
}

/***
 * SymbolsSetLabel()
 *   udela z identifikatoru label
 *   1. parametr - adresa symbolu vracena funkci SymbolsInsert()
 *   2. parametr - index do pasky, kam label ukazuje
 *   ! pri pokusu o redefinici labelu vraci nenulovou hodnotu, jinak 0
 */
int SymbolsSetLabel(PSymbol symbol, unsigned index)
{
#ifndef NDEBUG
  if(symbol == NULL)
    ErrorFatal("Internal error - NULL symbol");
#endif
  if(symbol->type == stLabel)
    return(1/*+symbol->addr.LabelIndex*/);
  symbol->type = stLabel;
  symbol->addr.LabelIndex = index;
  return(0);
}

/***
 * SymbolDestroy()
 *   rekurzivne uvolnovani symbolu
 *   parametr - adresa korenoveho symbolu  
 */ 
void SymbolDestroy(PSymbol root)
{
  // NULL symbol = return z rekurze
  if(root == NULL)
    return;
  SymbolDestroy(root->LPtr);
  SymbolDestroy(root->RPtr);
  free(root->name);
  free(root);
}

/***
 * SymbolsDestroy()
 *   uvolni vsechny symboly + tabulku symbolu z pameti
 *   parametr - adresa tabulky symbolu  
 */ 
void SymbolsDestroy(PSymbols table)
{
#ifndef NDEBUG
  if(table == NULL)
    ErrorFatal("Internal error - no symbol table");
#endif
  SymbolDestroy(table->root);
  free(table);
}

/***
 * SymbolsGetType()
 *   funkce pro runtime, kt. vrati typ symbolu
 *   parametr - adresa symbolu 
 */
TSymbolType SymbolsGetType(PSymbol symbol)
{
#ifndef NDEBUG
  if(symbol == NULL)
    ErrorFatal("Internal error - no symbol");
#endif
  return(symbol->type);
}

/***
 * SymbolsGetName()
 *   funkce pro runtime, kt. vrati jmeno symbolu (pro vypis chyb)
 *   parametr - adresa symbolu
 */
char *SymbolsGetName(PSymbol symbol)
{
#ifndef NDEBUG
  if(symbol == NULL)
    ErrorFatal("Internal error - no symbol");
#endif
  return(symbol->name);
}

/***
 * SymbolsGetLabelIndex()
 *   funkce pro runtime, kt. vrati index do pasky u labelu
 *   parametr - adresa symbolu  
 */
unsigned SymbolsGetLabelIndex(PSymbol symbol)
{
#ifndef NDEBUG
  if(symbol == NULL)
    ErrorFatal("Internal error - no symbol");
  if(symbol->type != stLabel)
    ErrorFatal("Internal error - symbol is no label");
#endif
  return(symbol->addr.LabelIndex);
}

/***
 * SymbolsGetVariable()
 *   funkce pro runtime, kt. vrati odkaz na promenou 
 *   parametr - adresa symbolu 
 */
PVariable SymbolsGetVariable(PSymbol symbol)
{
#ifndef NDEBUG
  if(symbol == NULL)
    ErrorFatal("Internal error - no symbol");
  if(symbol->type != stVariable)
    ErrorFatal("Internal error - symbol is no variable");
#endif
  return(symbol->addr.VariablePtr);
}
