/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    library.h
 * Popis:     Vestavene funkce
 * Datum:     3.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "kmp.h"
#include "quicksort.h"
#include "symbol_table.h"

E_CODE input (tSymbolData*);
E_CODE numeric (tSymbolData*, tSymbolData*);
void print (tSymbolData);
void typeOf (tSymbolData*, tSymbolData*);
void len (tSymbolData*, tSymbolData*);
void find (tSymbolData*, tSymbolData*, tSymbolData*);
E_CODE sort (tSymbolData*, tSymbolData*);

#endif
