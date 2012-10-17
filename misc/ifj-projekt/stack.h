/*
 * @description   Práce se dynamickým zásobníkem
 *                pozn. funkce nekontroluji platnost ukazatele na zasobnik
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define STACK_EOK     0
#define STACK_ERR    -1
#define STACK_EEMPTY -2
#define STACK_EALLOC -5

typedef struct tSItem {   // struktura pro polo¾ku v zásobníku
   struct tSItem *next;     // ukazatel na dal¹í polo¾ku
   void *data;             // ukazatel na data
} TSItem;

typedef struct {          // struktura pro zásobník
   TSItem *top;          // ukazatel na vrchol zásobníku
} TStack;


// Funkce pro práci se zásobníkem:

void stackInit     (TStack *s);
void stackDelete   (TStack *s);

inline void *stackTop    (TStack *s);
inline int stackPop      (TStack *s);
inline int stackPush     (TStack *s, void *data);

inline void *stackTopPop (TStack *s);

/////////////////////////////////////////////////////////////////
/* inline funkce */

/*
 * Vrátí nulu, pokud je zásobník prázdný, jinak nenulovou hodnotu.
 * Není o¹etøena platnost ukazatele na zásobník.
 * @param   ukazatel na zásobník
 * @return  true/false
 */
#define stackEmpty(S) ((S)->top == NULL)

/*
 * Vrátí ukazatel na data z polo¾ky na vrcholu zásobníku.
 * Pokud je zasobnik prazdny, vrátí se NULL.
 * @param   ukazatel na zásobník
 * @return  ukazatel na data nebo NULL
 */
inline void *stackTop (TStack *s) {

   if (!stackEmpty(s)) {
      return s->top->data;
   } else return NULL;

}

/*
 * Vyjme polo¾ku na vrcholu zásobníku.
 * Pokud je zásobník prázdný, vyvolá se chyba.
 * @param   ukazatel na zásobník
 * @return  kód chyby
 */
inline int stackPop (TStack *s) {

   if (!stackEmpty(s)) {
      TSItem *pom = s->top;
      s->top = s->top->next;
      free(pom);
   } else return STACK_EEMPTY; // pøístup do prázdného zásobníku

   return STACK_EOK;
}

/*
 * Vlo¾í na vrchol zásobníku novou polo¾ku s daty.
 * Pokud je nedostatek pamìti, vyvolá se chyba.
 * @param   ukazatel na zásobník
 * @param   ukazatel na data
 * @return  kód chyby
 */
inline int stackPush (TStack *s, void *data) {

   TSItem *elem;

   if ( (elem = (TSItem *)malloc(sizeof(TSItem))) != NULL ) {
      elem->data = data;
      elem->next = s->top;
      s->top = elem;

   } else return STACK_EALLOC; // nedostatek pamìti

   return STACK_EOK;
}

/*
 * Vrátí ukazatel na data na vrcholu zásobníku
 * a vyjme polo¾ku z vrcholu zásobníku.
 * Pokud dojde k chybì, vrátí NULL.
 * @param   ukazatel na zásobník
 * @return  ukazatel na data nebo NULL
 */
inline void *stackTopPop (TStack *s) {

   void *pom = stackTop(s);
   if ((pom == NULL) || (stackPop(s) != STACK_EOK)) {
      return NULL;
   } else return pom;
}


#endif // STACK_H_INCLUDED
