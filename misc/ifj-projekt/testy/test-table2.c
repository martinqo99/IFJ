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

void tiskniStack (TStack *s) {

   printf("Stav zásobníku:\n");

   TSItem *pom = s->top;

   while (pom != NULL) {
      printf("\t%s\n",((TVar*)(pom->data))->name);
      pom = pom->next;
   }
   printf("________________\n\n");
}

#define prazdnyStack(stack) printf("Je zásobník prázdný? %s\n",(stackEmpty(&stack))? "ANO" : "NE");


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

   // konec testu stack

   printf("\nSmazu: \n");
   tableClear(&table);
   tablePrintOrder(table);
   printf("\n----------------------------\n");
}
