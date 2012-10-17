/*
 * @description   Syntaktick� anal�za v�raz� (zdola nahoru)
 * @author        Vendula Poncov� - xponco00
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

// pomocn� konstanty:

#define EOK        0                  // chybov� k�d pro �sp�ch
#define SEM_ERR   -3                  // chybov� k�d pro s�mantickou chybu
#define ENDEXPR    END_OF_FILE + 1    // ukon�ova� �et�zce a dno z�sobn�ku
#define MAXTAB     ENDEXPR + 1        // rozm�r preceden�n� tabulky
#define EXPRESSION -1                 // nen� token
#define NOINSTR    -1                 // negenerovat instrukci
#define OFFSET     I_ADD - L_ADDITION // posun pro generov�n� instrukc�
#define DATTYPE     4                 // pocet datovych typu

// pomocn� makra pro tokeny:

#define isId(t)       (t == L_ID || t == KW_MAIN)
#define isBracket(t)  (t == L_LEFT_BRACKET || t == L_RIGHT_BRACKET)
#define isConst(t)    (t >= KW_TRUE        && t <= L_STRING)
#define isOperator(t) (t >= L_ADDITION     && t <= L_UNEQUAL)

// pomocn� makra pro typy instrukc�:

#define isMathOperation(i) (i >= I_ADD   && i <= I_CMP_NE) // instrukce matematicke i logicke
#define isLGEOperation(i)  (i >= I_CMP_L && i <= I_CMP_GE) // logicke instrukce: L,LE,G,GE

// tabulka pro kontrolu semantiky:

extern const int semTable[][DATTYPE];

// pomocn� struktura pro data na z�sobn�ku:

typedef struct {
   int token;            // token
   TVar *var;            // adresa prom�nn�
} TStackData;


// funkce pro parsov�n� v�razu:

int parseExpression (TTable *t, TVar **ptrResult);

int shift           (TStack *S, int token, TVar *pom);
int getTopTerminal  (TStack *S);
int getTopToken     (TStack *S);

int findRule        (TStack *S, TInstr *instr);
int checkRule       (TInstr *instr);

inline int checkSemErr     (TInstr *instr, TVarData *data);

int insertInstruction(TInstr *instr, TTable *table);
int returnResult    (TStack *S, TVar **ptrResult);

// funkce pro vytv��en� a maz�n� pomocn�ch struktur:

TVar       *createTmpVar    (TList *L, int *err);
TStackData *createStackData (int token, TVar *var, int *err);

void stackDataDelete (TStack *S);

// pomocn� v�pisy:

void tiskniStack (TStack *s);
void tiskniPrecTab();

///////////////////////////////////////////////////////////////////////////
/* inline funkce */

/*
 * Provede s�mantickou kontrolu pro prom�nnou a typ instrukce.
 * Kontrola se prov�d� pouze pro matematick� a rela�n� instr.
 * Ke kontrole slou�� tabulka semTable.
 * @param   ukazatel na instrukci
 * @param   ukazatel na data prom�nn�
 * @return  chybov� k�d
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
