/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    interpret.h
 * Popis:     Interpret prekladace jazyka IFJ12
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef INTERPRET_H_INCLUDED
#define INTERPRET_H_INCLUDED

#include "syntactic.h"
#include "symbol_table.h"
#include "library.h"
#include <math.h>
#include "quicksort.h"
#include "kmp.h"
E_CODE interpret(tSymbolTable *);
E_CODE interpret_recursive (tFunction *, tStack *);
bool isConstant(tSymbol *);
E_CODE copySymbolData(tSymbolData *,tSymbolData *);
void reallocSymbol(tSymbol *,tFunction *);
tSymbolData* getData(tSymbol*,tFunction *);

#endif
