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
const char PrecedentTable [MAXTABLE][MAXTABLE] =
{ // tabulka nemusi byt dobre, chce se to nekomu kontrolovat?
  // tokeny                         id  (   )   +   -   *   /   **  ==  !=  <   <=  >   >=
  [LEX_ID]              ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_L_BRACKET]       ={[LEX_ID]='<','<','=','<','<','<','<','<','<','<','<','<','<','<',[LEX_EOL]= 0 },
  [LEX_R_BRACKET]       ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_ADDITION]        ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_SUBSTRACTION]     ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
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
};
/**
 * @info      Nalezeni pravidla a ulozeni do instrukce
 * @param   tStack* - ukazatel na stack s hodnotama
 * @param   tInstr* - instrukce
 * @return  E_CODE - chybovy kod
 */
E_CODE findInstr (tStack *S, tInstr *instr)
{
  tExprData *help = NULL;

  if (stackEmpty(S) == true) return ERROR_SYNTAX;
  help = stackPop(S);
  if (help->kw == EXPR) { // E -> E operator E
    instr->src2 = help->token; // druhy E
    mmuFree(help);

    if (stackEmpty(S) == true) return ERROR_SYNTAX;
    help = stackPop(S);
    if (help->kw < LEX_ADDITION || help->kw > LEX_UNEQUAL) return ERROR_SYNTAX;
    instr->type = help->kw; // prideleni operandu
    free(help);

    if (stackEmpty(S) == true) return ERROR_SYNTAX;
    help = stackPop(S);
    if (help->kw != EXPR) return ERROR_SYNTAX;
    instr->src1 = help->token; // prvni E
    mmuFree(help);
  }
  else if (help->kw == LEX_R_BRACKET) { // E -> (E)
    instr->type = EXPR;
    mmuFree(help);

    if (stackEmpty(S) == true) return ERROR_SYNTAX;
    help = stackPop(S);
    if (help->kw != EXPR) return ERROR_SYNTAX;
    instr->dest = help->token; // (E)
    mmuFree(help);

    if (stackEmpty(S) == true) return ERROR_SYNTAX;
    help = stackPop(S);
    if (help->kw != LEX_L_BRACKET) return ERROR_SYNTAX;
    mmuFree(help);
  }
  else return ERROR_SYNTAX;

  return ERROR_OK;
}

/**
 * @info      Semanticka kontrola vyrazu
 * @param   tInstr* - instrukce
 * @return  E_CODE - chybovy kod
 */
E_CODE checkInstr(tInstr *instr)
{
  if (instr->type == EXPR) return ERROR_OK;
  else if (instr->type >= I_ADD && instr->type <= I_POW) {
    if (((tSymbol*)(instr->src1))->type != DT_NUMBER) return ERROR_SEMANTIC;
    if (((tSymbol*)(instr->src2))->type != DT_NUMBER) return ERROR_SEMANTIC;
  }

  return ERROR_OK;
}

/**
 * @info      Vlozeni instrukce do tabulky
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @param   tInstr* - instrukce
 * @return  E_CODE - chybovy kod
 */
E_CODE insertInstr(tSymbolTable *table, tInstr *instr)
{
  if (instr->type == EXPR) return ERROR_OK;

  tSymbol *help = insertBlankConstant(table->currentFunc);
  if (help == NULL) return ERROR_COMPILATOR;
  instr->dest = help;

  tInstr *instruction = genInstr(instr->type, instr->dest, instr->src1, instr->src2);
  if (instruction == NULL) return ERROR_COMPILATOR;
  return listInsertLast(&(table->currentFunc->instructions), instruction);
}

/**
 * @info      Vyhodnoceni vyrazu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @param   tKeyword - uz nactene klicove slovo
 * @param   tSmybol* - semka ulozime vysledek
 * @return  E_CODE - chybovy kod
 */
E_CODE prsExpression (tSymbolTable *table, tKeyword a, tSymbol **result)
{
  if (a == KW_IF || a == KW_WHILE || a == KW_RETURN)
    a = getToken(); // aby jsme se dostali do stejneho stavu, jako kdyz se sem dostaneme z ostatnich stavu

  tExprData *help;
  if ((help = mmuMalloc(sizeof(tExprData))) == NULL) return ERROR_COMPILATOR;

  help->kw = a;
  help->token = NULL;
  if (a == LEX_ID) {
    if ((help->token = functionSearchSymbol(table->currentFunc, gToken.data)) == NULL)
      return ERROR_SEMANTIC_VARIABLE;
    help->kw = LEX_EOL;
  }
  else if (a == LEX_NUMBER || a == LEX_STRING) {
    if ((help->token = functionInsertConstant(table->currentFunc, gToken.data, a)) == NULL)
      return ERROR_COMPILATOR;
    help->kw = LEX_EOL;
  }
  //else if (a >= LEX_L_BRACKET && a <= LEX_UNEQUAL) // asi nebude treba


  E_CODE err = ERROR_OK;
  tKeyword b = EXPR;
  tInstr instr = {EXPR, NULL, NULL, NULL};
  char x; // vysledek hledani v tabulce
  tStack *S = stackCreate();
  err = stackInit(S);

  err = stackPush(S, help);

  while ((a != LEX_EOL || b != LEX_EOL) && err == ERROR_OK) {
    tStackPtr save = S->top;
    while (save != NULL && ((tExprData*)(S->top->data))->kw == EXPR) save = S->top->ptr;
    if (save == NULL) b = EXPR;
    else b = ((tExprData*)(save->data))->kw;
    a = getToken();

    help->kw = a;
    help->token = NULL;
    if (a == LEX_ID) {
      if ((help->token = functionSearchSymbol(table->currentFunc, gToken.data)) == NULL)
        return ERROR_SEMANTIC_VARIABLE;
      help->kw = EXPR;
    }
    else if (a == LEX_NUMBER || a == LEX_STRING) {
      if ((help->token = functionInsertConstant(table->currentFunc, gToken.data, a)) == NULL)
        return ERROR_COMPILATOR;
      help->kw = EXPR;
    }
    //else if (a >= LEX_L_BRACKET && a <= LEX_UNEQUAL) // asi nebude treba

    if (a == LEX_NUMBER || a == LEX_STRING) // nalezeni x v tabulce
      x = PrecedentTable[b][LEX_ID];
    else
      x = PrecedentTable[b][a];

    if (x == 0) return ERROR_SYNTAX; // chyba
    else if (x == '=' || x == '<') { // nic se nedeje, jen push
      err = stackPush(S, help);
    }
    else if (x == '>') { // jediny kde se neco deje
      // tady to bude kurevsky cerna magie
      if ((err = findInstr(S, &instr)) != ERROR_OK) return err;
      //if ((err = checkInstr(&instr)) != ERROR_OK) return err;
      if ((err = insertInstr(table, &instr)) != ERROR_OK) return err;

      help->kw = EXPR;
      help->token = instr.dest;
      err = stackPush(S, help);
    }
    else if (x == '$') { // konecna
      if (stackEmpty(S) == true) return ERROR_SYNTAX;
      help = stackPop(S);
      if (help->kw == EXPR)
        *result = help->token;
      else return ERROR_SYNTAX;
    }
    else return ERROR_SYNTAX; // cokoli jinyho je spatne, nemuze nastat
  } // konec while cyklu /////////////////////////////////

  mmuFree(help);
  stackDispose(S);
  stackDestroy(S);

  return err;
}
