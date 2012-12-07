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
#define MAXTABLE 16
#define EXPR -1

typedef struct texprdata
{
  tKeyword kw;
  tSymbol *token;
} tExprData;

const char precedentTable [MAXTABLE][MAXTABLE] =
{ // tabulka nemusi byt dobre, chce se to nekomu kontrolovat?
  // tokeny                         id  (   )   +   -   *   /   **  ==  !=  <   <=  >   >=
  [LEX_ID]              ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_L_BRACKET]       ={[LEX_ID]='<','<','=','<','<','<','<','<','<','<','<','<','<','<',[LEX_EOL]= 0 },
  [LEX_R_BRACKET]       ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_ADDITION]        ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_SUBTRACTION]     ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_MULTIPLICATION]  ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_DIVISION]        ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_POWER]           ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_EQUAL]           ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_UNEQUAL]         ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_LESSER]          ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_LESSER_EQUAL]    ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_GREATER]         ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_GREATER_EQUAL]   ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_EOL]             ={[LEX_ID]='<','<', 0 ,'<','<','<','<','<','<','<','<','<','<','<',[LEX_EOL]='$'},
}

E_CODE prsExpression (tSymbolTable *table, tKeyword kw, tSymbol **result);

#endif
