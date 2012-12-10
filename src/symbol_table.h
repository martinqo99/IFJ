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
#include "scanner.h"

#define TRUE true
#define FALSE false

typedef enum tdatatype{
  DT_NIL,
  DT_BOOL,
  DT_NUMBER,
  DT_STRING,
  DT_FUNCTION,//pro TypeOf
  DT_UNKNOWN
} tDataType;

typedef union tdata{
  int iData;
  bool bData;
  double dData;
  tString sData;
} tData;

typedef struct tsymboldata{
  tDataType type;
  tData data;
} tSymbolData;

typedef struct tsymbol{
  tString key;
  tSymbolData *data; //pole, pro vice instanci stejne promenne
  tDataType type;
} tSymbol;

typedef struct tfunction{
    tString name;

    tBTree symbols;

    bool declared;
    int called;
    tList constants;
    tList instructions;
} tFunction;

typedef enum {
    I_RETURN,
//stack
    I_POP,
    I_PUSH,
    I_SEMPTY,
//presun
    I_MOV,
//aritmetika
    I_ADD, //= LEX_ADDITION,
    I_SUB, //= LEX_SUBSTRACTION,
    I_MUL, //= LEX_MULTIPLICATION,
    I_DIV, //= LEX_DIVISION,
    I_POW, //= LEX_POWER,
//logika
    I_EQUAL, //= LEX_EQUAL,		// ==
    I_NEQUAL, //= LEX_UNEQUAL,	// !=
    I_LESS, //= LEX_LESSER, //<
    I_ELESS, //= LEX_LESSER_EQUAL,		// <=
    I_MORE, //= LEX_GREATER,		// >
    I_EMORE, //= LEX_GREATER_EQUAL,		// >=
//skoky - snad nebudou potreba vsechny
    I_LABEL,		// navesti
    I_FJUMP,		//false jump
    I_JUMP,		//nepodmineny jump

    I_CALL,
//vestavene fce
    I_INPUT,
    I_NUMERIC,
    I_PRINT,
    I_TYPEOF,
    I_LEN,
    I_FIND,
    I_SORT,
    I_STRING, // dest od do, string je na stacku
    NOINSTR
}tItype;

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



/*
 * generuje instrukci
 * @param   tabulka symbolu
 * @param   typ instrukce
 * @param   dst
 * @param   src1
 * @param   src2
 * @return  ukazatel na instrukci
 */
tInstr* genInstr(tItype, void*, void*, void*);
tSymbol* functionInsertConstant(tFunction*,tString,tKeyword);
void symbolTableInit(tSymbolTable*);
E_CODE symbolTableInsertFunction(tSymbolTable*, tString);
tFunction* symbolTableSearchFunction(tSymbolTable*, tString);
void symbolTableDispose(tSymbolTable*);
tSymbol* functionSearchSymbol(tFunction*, tString);
E_CODE functionInsertSymbol(tFunction*,tString);
tSymbol* getLastSymbol(tFunction*);
tSymbol* getLastConstant(tFunction*);
tSymbol* insertBlankConstant(tFunction*);

#endif
