/*
 * @description   TEST: Práce se dynamickým zásobníkem
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "stack.h"

/**
 * Funkce pro pomocné výpisy:
 */

void tiskniZasobnik (tStack *s) {

   printf("Stav zásobníku:\n");

   tElemPtr pom = s->top;

   while (pom != NULL) {
      printf("\t%d\n",pom->data);
      pom = pom->ptr;
   }
   printf("________________\n\n");
}

void prazdnyZasobnik (tStack *s) {

   if (stackEmpty(s)) printf("Zásobník je prázdný.\n");
   else               printf("Zásobník NENÍ prázdný. \n");

}

/**
 * Hlavní program.
 */
int main(void) {
   tStack s;
   stackInit(&s);
   tiskniZasobnik(&s);
   prazdnyZasobnik(&s);

   stackPush(&s, 1);
   tiskniZasobnik(&s);
   prazdnyZasobnik(&s);

   stackPush(&s, 2);
   stackPush(&s, 3);
   stackPush(&s, 4);

   tiskniZasobnik(&s);
   prazdnyZasobnik(&s);

   int pom = 0;
   stackTop(&s, &pom);
   stackPop(&s);
   printf("Vyjmuli jsme: %d\n",pom);

   stackTop(&s, &pom);
   stackPop(&s);
   printf("Vyjmuli jsme: %d\n",pom);

   tiskniZasobnik(&s);
   prazdnyZasobnik(&s);


   stackDelete(&s);

   tiskniZasobnik(&s);
   prazdnyZasobnik(&s);


   return EXIT_SUCCESS;
}

/* konec stack_test.c */
