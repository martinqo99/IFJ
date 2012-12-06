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
E_CODE prsExpression (tSymbolTable *table, tKeyword a, tSymbol *result)
{
  if (a == KW_IF || a == KW_WHILE || a == KW_RETURN)
    a = getToken(); // aby jsme se dostali do stejneho stavu, jako kdyz se sem dostaneme z ostatnich stavu

  tExprData *help;
  if ((help = mmuMalloc(sizeof(tExprData))) == NULL) return ERROR_COMPILATOR;

  help->kw = a;
  help->token = NULL;
  if (a == LEX_ID) {
    if ((help->token = functionSearchSymbol(table->currentFunc, gToken->data)) == NULL)
      return ERROR_SYNTAX;
    help->kw = EXPR;
  }
  else if (a == LEX_NUMBER || a == LEX_STRING) {
    if ((help->token = functionInsertConstant(table->currentFunc, gToken.data, a)) == NULL)
      return ERROR_COMPILATOR;
    help->kw = EXPR;
  }
  //else if (a >= LEX_L_BRACKET && a <= LEX_UNEQUAL) // asi nebude treba


  E_CODE err = ERROR_OK;
  tKeyword b = EXPR;
  char x; // vysledek hledani v tabulce
  tStack *S = stackCreate();
  err = stackInit(S);

  err = stackPush(S, help);

  while ((a != LEX_EOL || b != LEX_EOL) && err == ERROR_OK) {
    b = stackTop(S);
    a = getToken();

    help->kw = a;
    help->token = NULL;
    if (a == LEX_ID) {
      if ((help->token = functionSearchSymbol(table->currentFunc, gToken->data)) == NULL)
        return ERROR_SYNTAX;
      help->kw = EXPR;
    }
    else if (a == LEX_NUMBER || a == LEX_STRING) {
      if ((help->token = functionInsertConstant(table->currentFunc, gToken.data, a)) == NULL)
        return ERROR_COMPILATOR;
      help->kw = EXPR;
    }
    //else if (a >= LEX_L_BRACKET && a <= LEX_UNEQUAL) // asi nebude treba

    if (a == LEX_NUMBER || a == LEX_STRING)
      x = precedentTable[b][LEX_ID];
    else
      x = precedentTable[b][a];

    if (x == 0) return ERROR_SYNTAX;
    else if (x == '=' || x == '<') {
      err = stackPush(S, help);
    }
    else if (x == '>') {
      // tady to bude kurevsky cerna magie
      err = stackPush(S, help);
    }
    else if (x == '$') {
      help = stackPop(S);
      if (help->kw == EXPR)
        result = help->token;
      else return ERROR_SYNTAX;
    }
    else return ERROR_SYNTAX;
  } // konec while cyklu /////////////////////////////////

  mmuFree(help);
  err = stackDispose(S);
  err = stackDestroy(S);

  return err;
}
