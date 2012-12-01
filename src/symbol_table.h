/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   symbol_table.h
 *
 * Popis:
 *
 *
 * Datum:    20.11.2012
 *
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

#include "strings.h"
#include "binary_tree.h"

typedef enum tdatatype{
  DT_NIL,
  DT_BOOL,
  DT_NUMBER,
  DT_STRING
} tDataType;

typedef union tdata{
  int iData;
  double dData;
  tString sData;
} tData;

typedef struct tsymboldata{
  tDataType type;
  tData data;
} tSymbolData;

typedef struct tsymbol{
  tString key;
  tSymbolData data;
} tSymbol;

typedef struct tlistinstruction {
  // kdovi co
  tListInstruction *next;
} tListInstruction;

typedef struct tfunction{
    tString name;

    tBTree symbols;

    bool declared;
    int called;

    tListInstruction *instruction;
} tFunction;

typedef struct tsymboltable{
    tBTree functions;
} tSymbolTable;


void symbolTableInit(tSymbolTable*);
void symbolTableInsertFunction(tSymbolTable*, tString);
tFunction* symbolTableSearchFunction(tSymbolTable*, tString);
void symbolTableDispose(tSymbolTable*);

#endif
