/*
 * @description   Práce s jednosmìrným seznamem
 *                pozn. funkce nekontroluji platnost ukazatele na seznam
 * @author        Vendula Poncová - xponco00
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

typedef struct tLItem {      // struktura pro polo¾ku seznamu
   struct tLItem *next;       // ukazatel na dal¹í polo¾ku
   void *data;                // ukazatel na data
} TLItem;

typedef struct {             // struktura pro seznam:
   TLItem *Act;               // uk na aktivní prvek
   TLItem *First;             // uk na první prvek
   TLItem *Last;              // uk na poslední prvek
} TList;


// Funkce pro práci se seznamem:

void listInit         (TList *L);
void listDispose      (TList *L);

int listInsertFirst  (TList *L, void *data);
int listInsertLast   (TList *L, void *data);
int listPostInsert   (TList *L, void *data);

int listDeleteFirst  (TList *L);
int listPostDelete   (TList *L);  // neimplementováno

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
 * Vrátí true, pokud se seznam aktivní, jinak false.
 * Nehlídá platnost ukazatele!
 * @param   ukazatel na seznam
 * @return  true/false
 */
#define listActive(L)  ((L)->Act != NULL)

/*
 * Aktivitu seznamu nastaví na první polo¾ku.
 * @param   ukazatel na seznam
 */
#define listFirst(L)  { (L)->Act = (L)->First; }

/*
 * Aktivitu seznamu nastaví na poslední polo¾ku.
 * @param   ukazatel na seznam
 */
#define listLast(L)  { (L)->Act = (L)->Last; }

/*
 * Posune aktivitu na následující polo¾ku seznamu.
 * Pokud seznam není aktivní, nestane se nic.
 * @param   ukazatel na seznam
 */
#define listSucc(L)  { if((L)->Act != NULL) (L)->Act = (L)->Act->next; }

/*
 * Vrátí ukazatel na data z aktivní polo¾ky.
 * Neaktivní seznam vrátí NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopy(L)  ( ((L)->Act != NULL) ? (L)->Act->data : NULL)

/*
 * Vrátí ukazatel na data v první polo¾ce.
 * Pokud je seznam prázdný, vrátí se NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopyFirst(L)  ( ((L)->First != NULL) ? (L)->First->data : NULL)

/*
 * Vrátí ukazatel na data v poslední polo¾ce.
 * Pokud je seznam prázdný, vrátí se NULL.
 * @param   ukazatel na seznam
 * @return  ukazatel na data nebo NULL
 */
#define listCopyLast(L)  ( ((L)->Last != NULL) ? (L)->Last->data : NULL)

/*
 * Vrátí ukazatel na aktivní prvek.
 * @param   ukazatel na seznam
 * @return  ukazatel na prvek seznamu
 */
#define listGetActive(L) ((L)->Act)

/*
 * Nastaví daný prvek na aktivní.
 * @param   ukazatel na seznam
 * @param   ukazatel na prvek seznamu
 */
#define listSetActive(L,uk) { (L)->Act = uk;}

#endif // LIST_H_INCLUDED
