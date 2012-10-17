/*
 * @description   Práce se dynamickým zásobníkem
 *                pozn. funkce nekontroluji platnost ukazatele na zasobnik
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "stack.h"

/*
 * Inicializuje se zásobník.
 * @param   ukazatel na zásobník
 */
void stackInit (TStack *s) {
      s->top = NULL;
}

/*
 * Vyjme ze zásobníku v¹echny polo¾ky.
 * Voláním tento funkce se zásobník stane prázdným.
 * @param   ukazatel na zásobník
 */
void stackDelete (TStack *s) {

  while (!stackEmpty(s)) {
    stackPop(s);
  }
}

/* konec stack.c */
