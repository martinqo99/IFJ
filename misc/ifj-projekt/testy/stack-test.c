/*
 * @description   TEST: Pr�ce se dynamick�m z�sobn�kem
 * @author        Vendula Poncov� - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "stack.h"

/**
 * Funkce pro pomocn� v�pisy:
 */

void tiskniZasobnik (tStack *s) {

   printf("Stav z�sobn�ku:\n");

   tElemPtr pom = s->top;

   while (pom != NULL) {
      printf("\t%d\n",pom->data);
      pom = pom->ptr;
   }
   printf("________________\n\n");
}

void prazdnyZasobnik (tStack *s) {

   if (stackEmpty(s)) printf("Z�sobn�k je pr�zdn�.\n");
   else               printf("Z�sobn�k NEN� pr�zdn�. \n");

}

/**
 * Hlavn� program.
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
