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
    switch ((tInstr* help->data)->type) {
      case I_RETURN:
        break;

      case I_POP:
        help->data->dest = stackPop(stack);
        //mozna jeste neco dalsiho :)
        break;

      case I_PUSH:
        err = stackPush(stack, (tInstr* help->data)->src1);
        // ze by neco vic?
        break;

      case I_SEMPTY:
        err = stackDispose(stack);
        break;

      case I_MOV:
        help->data->dest = help->data->src1;
        break;

      case I_SET:
        break;

      case I_ADD:
        if (help->data->src1->type == DT_NUMBER && help->data->src1->type == DT_NUMBER)
          help->data->dest->data->data.dData = help->data->src1->data->data.dData + help->data->src2->data->data.dData;
        else if (help->data->src1->type == DT_STRING && help->data->src1->type == DT_STRING) {
          int temp = help->data->src1->data->data.sData.len + help->data->src2->data->data.sData.len;
          help->data->dest->data->data.sData.data = mmuMalloc(sizeof(temp*char));
          if (help->data->dest->data->data.sData.data == NULL) return ERROR_COMPILATOR;
          help->data->dest->data->data.sData.data = strcat (help->data->src1->data->data.sData.data, help->data->src2->data->data.sData.data);
        }
        else if (help->data->src1->type == DT_NUMBER && help->data->src1->type == DT_STRING) {
          // tady nevim
        }
        else if (help->data->src1->type == DT_STRING && help->data->src1->type == DT_NUMBER) {
          // tady nevim
        }
        else return ERROR_SEMANTIC;
        break;

      case I_SUB:
        if (help->data->src1->type == DT_NUMBER && help->data->src1->type == DT_NUMBER)
          help->data->dest->data->data.dData = help->data->src1->data->data.dData - help->data->src2->data->data.dData;
        else return ERROR_SEMANTIC;
        break;

      case I_MUL:

    }
    help = help->next;
  }
}
