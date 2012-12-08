/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    interpret.c
 * Popis:     Interpret prekladace jazyka IFJ12
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "interpret.h"

/**
 * @info      Interpret
 * @param   tInstr - instrukce v 3 adresnym kodu
 * @return  E_CODE - chybovy kod
 */
E_CODE interpret (tList instr)
{
  tListItem *help = instr.first;
  E_CODE err = ERROR_OK;
  tStack *stack;
  err = stackInit(stack);

  while (help != NULL && err == ERROR_OK) {
    instrPtr = (tInstr*) help->data;
    src1SymbPtr = (tSymbol*) intrPtr->src1;
    src2SymbPtr = (tSymbol*) intrPtr->src2;
    destSymbPtr = (tSymbol*) intrPtr->dest; // pomocny ukazatele

    switch ((tInstr* help->data)->type) {
      case I_RETURN:
        break;

      case I_POP:
        destSymbPtr = stackPop(stack);
        //mozna jeste neco dalsiho :)
        break;

      case I_PUSH:
        err = stackPush(stack, src1SymbPtr);
        // ze by neco vic?
        break;

      case I_SEMPTY:
        err = stackDispose(stack);
        break;

      case I_MOV:
        destSymbPtr = src1SymbPtr;
        break;

      case I_SET:
        break;

      case I_ADD:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.dData = src1SymbPtr->data->data.dData + src2SymbPtr->data->data.dData;
        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING) {
          int temp = src1SymbPtr->data->data.sData.len + src2SymbPtr->data->data.sData.len;
          destSymbPtr->data->data.sData.data = mmuMalloc(sizeof(temp*char));
          if (destSymbPtr->data->data.sData.data == NULL) return ERROR_COMPILATOR;
          destSymbPtr->data->data.sData.data = strcat (src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data);
        }
        else if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_STRING) {
          // tady nevim
        }
        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_NUMBER) {
          // tady nevim
        }
        else return ERROR_SEMANTIC;
        break;

      case I_SUB:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.dData = src1SymbPtr->data->data.dData - src2SymbPtr->data->data.dData;
        else return ERROR_SEMANTIC;
        break;

      case I_MUL:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.dData = src1SymbPtr->data->data.dData * src2SymbPtr->data->data.dData;
        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_NUMBER) {
          // n-ta mocnina retezce, nevim
        }
        else return ERROR_SEMANTIC;
        break;

      case I_DIV:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.dData = src1SymbPtr->data->data.dData / src2SymbPtr->data->data.dData;
        else return ERROR_SEMANTIC;
        break;

      case I_POW:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.dData = pow(src1SymbPtr->data->data.dData, src2SymbPtr->data->data.dData);
        else return ERROR_SEMANTIC;
        break;

      case I_EQUAL:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData == src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) == 0)
            destSymbPtr->data->data.bData = true;
          else
            destSymbPtr->data->data.bData = false;

        else if (src1SymbPtr->type == DT_BOOL && src2SymbPtr->type == DT_BOOL)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData == src2SymbPtr->data->data.dData);

        else
          destSymbPtr->data->data.bData = false;
        break;

      case I_NEQUAL:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData != src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) == 0)
            destSymbPtr->data->data.bData = false;
          else
            destSymbPtr->data->data.bData = true;

        else if (src1SymbPtr->type == DT_BOOL && src2SymbPtr->type == DT_BOOL)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData != src2SymbPtr->data->data.dData);

        else
          destSymbPtr->data->data.bData = false;
        break;

      case I_LESS:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData < src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) < 0)
            destSymbPtr->data->data.bData = true;
          else
            destSymbPtr->data->data.bData = false;

        else
          destSymbPtr->data->data.bData = false;
        break;

      case I_ELESS:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData <= src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) <= 0)
            destSymbPtr->data->data.bData = true;
          else
            destSymbPtr->data->data.bData = false;

        else
          destSymbPtr->data->data.bData = false;
        break;

      case I_MORE:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData > src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) > 0)
            destSymbPtr->data->data.bData = true;
          else
            destSymbPtr->data->data.bData = false;

        else
          destSymbPtr->data->data.bData = false;
        break;

      case I_EMORE:
        if (src1SymbPtr->type == DT_NUMBER && src2SymbPtr->type == DT_NUMBER)
          destSymbPtr->data->data.bData = (src1SymbPtr->data->data.dData >= src2SymbPtr->data->data.dData);

        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_STRING)
          if (strcmp(src1SymbPtr->data->data.sData.data, src2SymbPtr->data->data.sData.data) >= 0)
            destSymbPtr->data->data.bData = true;
          else
            destSymbPtr->data->data.bData = false;

        else
          destSymbPtr->data->data.bData = false;
        break;

      case
    }
    help = help->next;
  }
}
