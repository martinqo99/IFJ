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
  DT_STRING,
  DT_UNKNOWN
} tDataType;

typedef union tdata{
  int iData;
  double dData;
  tString sData;
} tData;

/** Darek: toto je podle me zbytecny 
typedef struct tsymboldata{
  tDataType type;
  tData data;
} tSymbolData; */

typedef struct tsymbol{
  tString key;
  tData data;
  tDataType type;
} tSymbol;

typedef struct tfunction{
    tString name;

    tBTree symbols;

    bool declared;
    int called;

    tList instructions;
} tFunction;

typedef struct {
   int type;//bude ENUM
   void *dest;
   void *src1;
   void *src2;
} tInstr;

typedef struct tsymboltable{
    tBTree functions;
    tFunction mainFunc;
    tFunction *currentFunc;
} tSymbolTable;


void symbolTableInit(tSymbolTable*);
void symbolTableInsertFunction(tSymbolTable*, tString);
tFunction* symbolTableSearchFunction(tSymbolTable*, tString);
void symbolTableDispose(tSymbolTable*);

#endif
