/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    expression.c
 * Popis:     Vyhodnoceni vyrazu
 * Datum:     1.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "expression.h"

/**
 * @info      Vyhodnoceni vyrazu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @param   tKeyword - uz nactene klicove slovo
 * @return  E_CODE - chybovy kod
 */
E_CODE prsExpression (tSymbolTable *table, tKeyword kw)
{
  if (kw == KW_IF || kw == KW_WHILE || kw == KW_RETURN)
    kw = getToken();

  E_CODE err = ERROR_OK;
  tKeyword a = -1, b = -1;
  char x; // vysledek hledani v tabulce
  tStack *S = stackCreate();
  err = stackInit(S);

  err = stackPush(S, &kw);

  while ((a != LEX_EOL || b != LEX_EOL) && err == ERROR_OK) {
    b = stackTop(S);
    a = getToken();
    if (a == LEX_ID)
      if (functionSearchSymbol(table->currentFunc, gToken->data) == NULL)
        return ERROR_SYNTAX;
    else if (a == LEX_NUMBER || a == LEX_STRING)
      // pridani do stromu/listu konstant
    else if (a >= LEX_L_BRACKET && a <= LEX_UNEQUAL)
      // pridani nekam jinam
    x = precedentTable[b][a];

    if (x == 0) return ERROR_SYNTAX;
    else if (x == '=' || x == '<') {
      err = stackPush(S, &a);
    }
    else if (x == '>') {
      // tady to bude chtit vymyslet co presne je treba udelat
    }
    else if (x == '$') {
      // tady je konec vyrazu, ale vlastne k tomu nemuze dojit...fuck
    }
    else return ERROR_SYNTAX;

  } // konec while cyklu /////////////////////////////////

  err = stackDispose(S);
  err = stackDestroy(S);

  return err;
}
