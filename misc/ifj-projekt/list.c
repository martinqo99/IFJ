/*
 * @description   Pr�ce se jednosm�rn�m seznamem
 *                pozn. funkce nekontroluji platnost ukazatele na seznam
 * @author        Vendula Poncov� - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "list.h"

/*
 * Inicializuje se seznam.
 * @param   ukazatel na seznam
 */
void listInit (TList *L) {

   L->Act = NULL;
   L->First = NULL;
   L->Last = NULL;
}

/*
 * Zru�� v�echny prvky seznamu a inicializuje seznam.
 * @param   ukazatel na seznam
 */
void listDispose (TList *L) {

   TLItem *pom = NULL;
   L->Act = NULL;
   L->Last = NULL;

   while (L->First != NULL) {  // maz�n� prvk�
      pom = L->First;
      L->First = L->First->next;
      free(pom);
   }
}

/*
 * Vlo�� polo�ku s ukazatelem na data na za��tek seznamu.
 * Nedostatek pam�ti vyvol� chybu.
 * @param   ukazatel na seznam
 * @param   ukazatel na data
 * @return  k�d chyby
 */
int listInsertFirst  (TList *L, void *data) {

   TLItem *pom = NULL;          // alokace nov�ho prvku
   if ( (pom = (TLItem*)malloc(sizeof(TLItem))) != NULL ) {

      pom->data = data;         // inicializace
      pom->next = L->First;
      L->First = pom;

      // prvn� prvek je i posledn�
      if (L->First->next == NULL) L->Last = pom;
 
   } else return LIST_EALLOC;  // nedostatek pam�ti

   return LIST_EOK;
}

/*
 * Vlo�� polo�ku s ukazatelem na data na konec seznamu.
 * Nedostatek pam�ti vyvol� chybu.
 * @param   ukazatel na seznam
 * @param   ukazatel na data
 * @return  k�d chyby
 */
int listInsertLast  (TList *L, void *data) {

   TLItem *pom = NULL;        // alokace nov�ho prvku
   if ( (pom = (TLItem*)malloc(sizeof(TLItem))) != NULL ) {

      pom->data = data;         // inicializace
      pom->next = NULL;

      if (L->First == NULL) {   // seznam byl pr�zdn�
        L->First = pom;         // nov� prvek je prvn�m
      } else {                  // v seznamu alespo� 1 prvek
         L->Last->next = pom;
      }

      L->Last = pom;            // nov� prvek je posledn�

   } else return LIST_EALLOC;  // nedostatek pam�ti

   return LIST_EOK;
}

/*
 * Vlo�� novou polo�ku s ukazatelem na data za aktivn�.
 * Nedostatek pam�ti vyvol� chybu.
 * @param   ukazatel na seznam
 * @param   ukazatel na data
 * @return  k�d chyby
 */
int listPostInsert (TList *L, void *data) {

   if (L->Act != NULL) {        // n�jak� prvek je aktivn�

      TLItem *pom = NULL;       // alokace
      if ( (pom = (TLItem*)malloc(sizeof(TLItem))) != NULL ) {

         pom->data = data;        // inicializace
         pom->next = L->Act->next;
         L->Act->next = pom;

         // nastaven� posledn�ho prvku
         if (L->Act == L->Last) L->Last = pom;

      } else return LIST_EALLOC; // nedostatek pam�ti
   }
   return LIST_EOK;
}

/*
 * Zru�� prvn� polo�ku seznamu, pokud byla aktivn�, aktivita se zru��.
 * Ukazatel na NULL vyvol� chybu.
 * @param   ukazatel na seznam
 * @return  k�d chyby
 */
int listDeleteFirst (TList *L) {

   if (L->First != NULL) {     // seznam nen� pr�zdn�

      if (L->First == L->Act) { // prvn� prvek je aktivn�
         L->Act = NULL;          // zru� aktivitu
      }

      if (L->First == L->Last) { // prvn� prvek je posledn�
         L->Last = NULL;         // posledn� zru��me
      }

      TLItem *pom = L->First;  // zru� prvek
      L->First = L->First->next;
      free (pom);
   }
   return LIST_EOK;
}

/*
 * P�ep�e ukazatel na data aktivn� polo�ky seznamu.
 * Neplatn� ukazatel na data vyvol� chybu.
 * @param   ukazatel na seznam
 * @param   ukazatel na data
 * @return  k�d chyby
 */
int listActualize (TList *L, void *data ) {

   if (data != NULL) {
      if (L->Act != NULL) {
         L->Act->data = data; // p�ep�e uk na data
      }
   } else return LIST_ERR;  // neplatn� ukazatel
   return LIST_EOK;
}

/* konec list.c */
