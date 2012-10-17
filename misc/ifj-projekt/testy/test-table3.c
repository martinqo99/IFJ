/*
 * @description   test tabulkz funkci
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "str.h"
#include <string.h>
#include "stack.h"
#include "list.h"


void tiskniStack (TStack *s) {

   printf("Stav zásobníku:\n");

   TSItem *pom = s->top;

   while (pom != NULL) {
      printf("\t%s\n",((TVar*)(pom->data))->name);
      pom = pom->next;
   }
   printf("________________\n\n");
}

void tiskniList (TList *L) {

   printf("Stav seznamu:\n");

   TLItem *pom = L->First;

   while (pom != NULL) {
      printf("\t%s",((TVar*)(pom->data))->name);
      if (pom == L->First) printf("\t<- prvni prvek");
      if (pom == L->Last) printf("\t<- posledni prvek");
      if (pom == L->Act) printf("\t<- aktivni prvek");
      printf("\n");
      pom = pom->next;
   }
   printf("________________\n\n");
}


#define prazdnyStack(stack) printf("Je zásobník prázdný? %s\n",(stackEmpty(&stack))? "ANO" : "NE");
#define aktivniList(List) printf("Je seznam aktivní? %s\n",(listActive(&List))? "ANO" : "NE");


int main() {
   TTable table;
   tableInit(&table);

   printf("\nTabulka by mela byt prazdna: \n");
   tablePrintOrder(table);
   printf("\n----------------------------\n");

   tableInsertFunction(&table, strCreateString("func1"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var1func1"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var2func1"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var3func1"));

   printf("\nJedna funkce: \n");
   tablePrintOrder(table);
   printf("\n----------------------------\n");

   tableInsertFunction(&table, strCreateString("func2"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var1func2"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var2func2"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var3func2"));

   printf("\nDve funkce: \n");
   tablePrintOrder(table);
   printf("\n----------------------------\n");

   tableInsertFunction(&table, strCreateString("func3"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var1func3"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var2func3"));
   functionInsertVar(table.lastAddedFunc, strCreateString("var3func3"));

   printf("\nVsechny: \n");
   tablePrintOrder(table);
   printf("\n----------------------------\n");

   // test heldani
   {

      TFunction *fceSearch;

      printf("\nObsahuje tabulka funkci %s? \t", "func1");
      fceSearch = tableSearchFunction(&table, strCreateString("func1"));
      if(fceSearch != NULL) {
         printf("ANO\n");
         printf("   Obsahuje funkce promenou %s?\t", "var1func1");
         if(functionSearchVar(fceSearch, strCreateString("var1func1")) != NULL)
            printf("ANO");
         else
            printf("NE");
      } else
         printf("NE\n");

      printf("\nObsahuje tabulka funkci %s? \t", "funcX");
      fceSearch = tableSearchFunction(&table, strCreateString("funcX"));
      if(fceSearch != NULL) {
         printf("ANO\n");
         printf("   Obsahuje funkce promenou %s?\t", "var1func1");
         if(functionSearchVar(fceSearch, strCreateString("var1func1")) != NULL)
            printf("ANO");
         else
            printf("NE");
      } else
         printf("NE\n");

      printf("\n----------------------------\n");
   }


   // test zásobníku:
   printf("TEST ZASOBNIKU:\n");
   printf("----------------------------\n");

   TStack s;
   stackInit(&s);

   prazdnyStack(s);

   TFunction *fce = tableSearchFunction(&table, strCreateString("func2"));
   TVar *id = functionSearchVar(fce, strCreateString("var1func2"));
   stackPush(&s, (void*)id);

   prazdnyStack(s);
   tiskniStack(&s);

   id = functionSearchVar(fce, strCreateString("var2func2"));
   stackPush(&s, (void*)id);
   tiskniStack(&s);
   id = functionSearchVar(fce, strCreateString("var3func2"));
   stackPush(&s, (void*)id);
   tiskniStack(&s);
   id = functionSearchVar(fce, strCreateString("var1func2"));
   stackPush(&s, (void*)id);
   tiskniStack(&s);


   TVar *data = (TVar*)stackTopPop(&s);
   if (data != NULL)  printf("Vybráno ze zásobníku: %s\n",data->name);
   else printf("Ukazatel je nulový! \n");
   prazdnyStack(s);

   data = (TVar*)stackTopPop(&s);
   if (data != NULL)  printf("Vybráno ze zásobníku: %s\n",data->name);
   else printf("Ukazatel je nulový! \n");
   prazdnyStack(s);

   data = (TVar*)stackTopPop(&s);
   if (data != NULL)  printf("Vybráno ze zásobníku: %s\n",data->name);
   else printf("Ukazatel je nulový! \n");
   prazdnyStack(s);

   tiskniStack(&s);
   stackDelete   (&s);
   prazdnyStack(s);
   tiskniStack(&s);

   data = (TVar*)stackTopPop(&s);
   if (data != NULL)  printf("Vybráno ze zásobníku: %s\n",data->name);
   else printf("Ukazatel je nulový! \n");
   printf("----------------------------\n");

   // test seznamu:
   printf("TEST SEZNAMU:\n");
   printf("----------------------------\n");


   TList L;
   listInit (&L);
   id = functionSearchVar(fce, strCreateString("var1func2"));
   TLItem *uk2 = listGetActive (&L);
   listSetActive(&L, uk2);
   aktivniList(L);

   listInsertLast(&L, id);
   tiskniList(&L);

   void *uk4 = listCopyLast(&L);
   printf("Zkopírováno:  %s\n",((TVar*)uk4)->name);


   aktivniList(L);
   listFirst(&L);
   tiskniList(&L);

   listLast(&L);
   tiskniList(&L);

   id = functionSearchVar(fce, strCreateString("var2func2"));
   listPostInsert(&L, id);
   tiskniList(&L);

   id = functionSearchVar(fce, strCreateString("var3func2"));
   listInsertLast(&L, id);
   tiskniList(&L);


   id = functionSearchVar(fce, strCreateString("var1func2"));
   listInsertLast(&L, id);
   tiskniList(&L);

   listFirst(&L);
   listSucc(&L);
   TLItem *uk = listGetActive (&L);
   tiskniList(&L);

   listLast(&L);
   tiskniList(&L);

   listSetActive(&L, uk);
   tiskniList(&L);

   aktivniList(L);
   listDeleteFirst(&L);
   tiskniList(&L);

   void *uk3 = listCopyLast(&L);
   printf("Zkopírováno:  %s\n",((TVar*)uk3)->name);
   listSucc(&L);
   tiskniList(&L);

   listActualize(&L, uk3);
   tiskniList(&L);

   listDispose (&L);
   printf("----------------------------\n");
   // konec testu seznamu

   printf("\nSmazu: \n");
   tableClear(&table);
   tablePrintOrder(table);
   printf("\n----------------------------\n");
}
