/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    expression.h
 * Popis:     Hlavickovy soubor pro vyrazy
 * Datum:     1.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include "scanner.h"
#include "stack.h"
#include "symbol_table.h"
#define MAXTABLE LEX_EOL+1
#define EXPR -1

typedef struct texprdata
{
  int kw;
  tSymbol *symbol;
} tExprData;



E_CODE prsExpression (tSymbolTable*, tKeyword, tSymbol**);
tExprData* newExprdata(tKeyword,tSymbol*);
tKeyword topTerm(tStack*);
E_CODE pushExprdata(tSymbolTable *,tStack *,tKeyword ,tSymbol *);
tExprData* newExprdata(tKeyword ,tSymbol *);
bool isOper(tKeyword);
tItype getItype(tKeyword);

#endif
