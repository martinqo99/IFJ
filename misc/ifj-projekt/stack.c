/*
 * @description   Pr�ce se dynamick�m z�sobn�kem
 *                pozn. funkce nekontroluji platnost ukazatele na zasobnik
 * @author        Vendula Poncov� - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "stack.h"

/*
 * Inicializuje se z�sobn�k.
 * @param   ukazatel na z�sobn�k
 */
void stackInit (TStack *s) {
      s->top = NULL;
}

/*
 * Vyjme ze z�sobn�ku v�echny polo�ky.
 * Vol�n�m tento funkce se z�sobn�k stane pr�zdn�m.
 * @param   ukazatel na z�sobn�k
 */
void stackDelete (TStack *s) {

  while (!stackEmpty(s)) {
    stackPop(s);
  }
}

/* konec stack.c */
