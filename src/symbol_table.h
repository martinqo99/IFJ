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
#include "list.h"
#include "strings.h"

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
   tItype type;//bude ENUM
   void *dest;
   void *src1;
   void *src2;
} tInstr;

typedef struct tsymboltable{
    tBTree functions;
    tFunction mainFunc;
    tFunction *currentFunc;
} tSymbolTable;

typedef enum {
    I_RETURN,
//stack
    I_POP,
    I_PUSH,
    I_SEMPTY,
//presun
    I_MOV,
    I_SET,
//aritmetika
    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    I_POW,
    I_CON,
//logika
    I_EQUAL,		// ==
    I_NEQUAL,	// !=
    I_ELESS,		// <=
    I_LESS,		// <
    I_MORE,		// >
    I_EMORE,		// >=
//skoky - snad nebudou potreba vsechny
    I_LABEL,		// jenom labeldsfds
    I_TJUMP,		//true jump
    I_FJUMP,		//false jump
    I_JUMP,		//nepodmineny jump
    I_GOTO,
    I_FCEJUMP,

//vestavene fce
    I_INPUT,
    I_NUMERIC,
    I_PRINT,
    I_TYPEOF,
    I_LEN,
    I_FIND,
    I_SORT,
    
}tItype;

/*
 * generuje instrukci
 * @param   kam instrukci ulozit
 * @param   typ instrukce
 * @param   dst
 * @param   src1
 * @param   src2
 * @return  ukazatel na instrukci
 */
E_CODE *genInstr(tFunction*,tItype, void*, void*, void*);
void symbolTableInit(tSymbolTable*);
E_CODE symbolTableInsertFunction(tSymbolTable*, tString);
tFunction* symbolTableSearchFunction(tSymbolTable*, tString);
void symbolTableDispose(tSymbolTable*);
tSymbol* functionSearchSymbol(tFunction*, tString);
E_CODE functionInsertSymbol(tFunction*,tString);
tSymbol* getLastSymbol(tFunction*);

#endif
