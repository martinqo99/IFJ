/*
 * @description   Pr�ce s jednosm�rn�m seznamem
 *                pozn. funkce nekontroluji platnost ukazatele na seznam
 * @author        Vendula Poncov� - xponco00
 * @projekt       IFJ11
 * @date
 */

#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define LIST_EOK     0
#define LIST_ERR    -1
#define LIST_EALLOC -5

typedef struct tLItem {      // struktura pro polo�ku seznamu
   struct tLItem *next;       // ukazatel na dal�� polo�ku
   void *data;                // ukazatel na data
} TLItem;

typedef struct {             // struktura pro seznam:
   TLItem *Act;               // uk na aktivn� prvek
   TLItem *First;             // uk na prvn� prvek
   TLItem *Last;              // uk na posledn� prvek
} TList;


// Funkce pro pr�ci se seznamem:

void listInit         (TList *L);
void listDispose      (TList *L);

int listInsertFirst  (TList *L, void *data);
int listInsertLast   (TList *L, void *data);
int listPostInsert   (TList *L, void *data);

int listDeleteFirst  (TList *L);
int listPostDelete   (TList *L);  // neimplementov�no

int listActualize    (TList *L, void *data );

//inline void   listFirst     (TList *L);
//inline void   listLast      (TList *L);
//inline void   listSucc      (TList *L);
//inline void *listCopy      (TList *L);
//inline void *listCopyFirst (TList *L);
//inline void *listCopyLast  (TList *L);
//inline int   listActive    (TList *L);

//inline TLItem *listGetActive (TList *L);
//inline void listSetActive (TList *L, TLItem *uk);

/////////////////////////////////////////////////////////////////
/*
 * Vr�t� true, pokud se seznam aktivn�, jinak false.
 * Nehl�d� platnost ukazatele!
 * @param   ukazatel na seznam
 * @return  true/false
 */
#define listActive(L)  ((L)->Act != NULL)

/*
 * Aktivitu seznamu nastav� na prvn� polo�ku.
 * @param   ukazatel na seznam
 */
#define listFirst(L)  { (L)->Act = (L)->First; }

/*
 * Aktivitu seznamu nastav� na posledn� polo�ku.
 * @param   ukazatel na seznam
 */
#define listLast(L)  { (L)->Act = (L)->Last; }

/*
 * Posune aktivitu na n�sleduj�c� polo�ku seznamu.
 * Pokud seznam nen� aktivn�, nestane se nic.
 * @param   ukazatel na seznam
 */
#define listSucc(L)  { if((L)->Act != NULL) (L)->Act = (L)->Act->next; }

/*
 * Vr�t� ukazatel na data z aktivn� polo�ky.
 * Neaktivn� seznam vr�t� NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopy(L)  ( ((L)->Act != NULL) ? (L)->Act->data : NULL)

/*
 * Vr�t� ukazatel na data v prvn� polo�ce.
 * Pokud je seznam pr�zdn�, vr�t� se NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopyFirst(L)  ( ((L)->First != NULL) ? (L)->First->data : NULL)

/*
 * Vr�t� ukazatel na data v posledn� polo�ce.
 * Pokud je seznam pr�zdn�, vr�t� se NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopyLast(L)  ( ((L)->Last != NULL) ? (L)->Last->data : NULL)

/*
 * Vr�t� ukazatel na aktivn� prvek.
 * @param   ukazatel na seznam
 * @return  ukazatel na prvek seznamu
 */
#define listGetActive(L) ((L)->Act)

/*
 * Nastav� dan� prvek na aktivn�.
 * @param   ukazatel na seznam
 * @param   ukazatel na prvek seznamu
 */
#define listSetActive(L,uk) { (L)->Act = uk;}

#endif // LIST_H_INCLUDED
