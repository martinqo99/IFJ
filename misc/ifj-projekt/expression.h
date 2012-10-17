/*
 * @description   Syntaktická analýza výrazù (zdola nahoru)
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "list.h"
#include "table.h"
#include "scanner.h"
#include "parser.h"

// pomocné konstanty:

#define EOK        0                  // chybový kód pro úspìch
#define SEM_ERR   -3                  // chybový kód pro sémantickou chybu
#define ENDEXPR    END_OF_FILE + 1    // ukonèovaè øetìzce a dno zásobníku
#define MAXTAB     ENDEXPR + 1        // rozmìr precedenèní tabulky
#define EXPRESSION -1                 // není token
#define NOINSTR    -1                 // negenerovat instrukci
#define OFFSET     I_ADD - L_ADDITION // posun pro generování instrukcí
#define DATTYPE     4                 // pocet datovych typu

// pomocná makra pro tokeny:

#define isId(t)       (t == L_ID || t == KW_MAIN)
#define isBracket(t)  (t == L_LEFT_BRACKET || t == L_RIGHT_BRACKET)
#define isConst(t)    (t >= KW_TRUE        && t <= L_STRING)
#define isOperator(t) (t >= L_ADDITION     && t <= L_UNEQUAL)

// pomocná makra pro typy instrukcí:

#define isMathOperation(i) (i >= I_ADD   && i <= I_CMP_NE) // instrukce matematicke i logicke
#define isLGEOperation(i)  (i >= I_CMP_L && i <= I_CMP_GE) // logicke instrukce: L,LE,G,GE

// tabulka pro kontrolu semantiky:

extern const int semTable[][DATTYPE];

// pomocná struktura pro data na zásobníku:

typedef struct {
   int token;            // token
   TVar *var;            // adresa promìnné
} TStackData;


// funkce pro parsování výrazu:

int parseExpression (TTable *t, TVar **ptrResult);

int shift           (TStack *S, int token, TVar *pom);
int getTopTerminal  (TStack *S);
int getTopToken     (TStack *S);

int findRule        (TStack *S, TInstr *instr);
int checkRule       (TInstr *instr);

inline int checkSemErr     (TInstr *instr, TVarData *data);

int insertInstruction(TInstr *instr, TTable *table);
int returnResult    (TStack *S, TVar **ptrResult);

// funkce pro vytváøení a mazání pomocných struktur:

TVar       *createTmpVar    (TList *L, int *err);
TStackData *createStackData (int token, TVar *var, int *err);

void stackDataDelete (TStack *S);

// pomocné výpisy:

void tiskniStack (TStack *s);
void tiskniPrecTab();

///////////////////////////////////////////////////////////////////////////
/* inline funkce */

/*
 * Provede sémantickou kontrolu pro promìnnou a typ instrukce.
 * Kontrola se provádí pouze pro matematické a relaèní instr.
 * Ke kontrole slou¾í tabulka semTable.
 * @param   ukazatel na instrukci
 * @param   ukazatel na data promìnné
 * @return  chybový kód
 */
inline int checkSemErr (TInstr *instr, TVarData *data) {

   if (isMathOperation(instr->type)) {
      if (semTable[instr->type][data->type] != 1) {
         return SEM_ERR;
      }
   }
   return EOK;
}


#endif // EXPRESSION_H_INCLUDED
