/*
 * @description   Pr�ce se dynamick�m z�sobn�kem
 *                pozn. funkce nekontroluji platnost ukazatele na zasobnik
 * @author        Vendula Poncov� - xponco00
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

typedef struct tSItem {   // struktura pro polo�ku v z�sobn�ku
   struct tSItem *next;     // ukazatel na dal�� polo�ku
   void *data;             // ukazatel na data
} TSItem;

typedef struct {          // struktura pro z�sobn�k
   TSItem *top;          // ukazatel na vrchol z�sobn�ku
} TStack;


// Funkce pro pr�ci se z�sobn�kem:

void stackInit     (TStack *s);
void stackDelete   (TStack *s);

inline void *stackTop    (TStack *s);
inline int stackPop      (TStack *s);
inline int stackPush     (TStack *s, void *data);

inline void *stackTopPop (TStack *s);

/////////////////////////////////////////////////////////////////
/* inline funkce */

/*
 * Vr�t� nulu, pokud je z�sobn�k pr�zdn�, jinak nenulovou hodnotu.
 * Nen� o�et�ena platnost ukazatele na z�sobn�k.
 * @param   ukazatel na z�sobn�k
 * @return  true/false
 */
#define stackEmpty(S) ((S)->top == NULL)

/*
 * Vr�t� ukazatel na data z polo�ky na vrcholu z�sobn�ku.
 * Pokud je zasobnik prazdny, vr�t� se NULL.
 * @param   ukazatel na z�sobn�k
 * @return  ukazatel na data nebo NULL
 */
inline void *stackTop (TStack *s) {

   if (!stackEmpty(s)) {
      return s->top->data;
   } else return NULL;

}

/*
 * Vyjme polo�ku na vrcholu z�sobn�ku.
 * Pokud je z�sobn�k pr�zdn�, vyvol� se chyba.
 * @param   ukazatel na z�sobn�k
 * @return  k�d chyby
 */
inline int stackPop (TStack *s) {

   if (!stackEmpty(s)) {
      TSItem *pom = s->top;
      s->top = s->top->next;
      free(pom);
   } else return STACK_EEMPTY; // p��stup do pr�zdn�ho z�sobn�ku

   return STACK_EOK;
}

/*
 * Vlo�� na vrchol z�sobn�ku novou polo�ku s daty.
 * Pokud je nedostatek pam�ti, vyvol� se chyba.
 * @param   ukazatel na z�sobn�k
 * @param   ukazatel na data
 * @return  k�d chyby
 */
inline int stackPush (TStack *s, void *data) {

   TSItem *elem;

   if ( (elem = (TSItem *)malloc(sizeof(TSItem))) != NULL ) {
      elem->data = data;
      elem->next = s->top;
      s->top = elem;

   } else return STACK_EALLOC; // nedostatek pam�ti

   return STACK_EOK;
}

/*
 * Vr�t� ukazatel na data na vrcholu z�sobn�ku
 * a vyjme polo�ku z vrcholu z�sobn�ku.
 * Pokud dojde k chyb�, vr�t� NULL.
 * @param   ukazatel na z�sobn�k
 * @return  ukazatel na data nebo NULL
 */
inline void *stackTopPop (TStack *s) {

   void *pom = stackTop(s);
   if ((pom == NULL) || (stackPop(s) != STACK_EOK)) {
      return NULL;
   } else return pom;
}


#endif // STACK_H_INCLUDED
