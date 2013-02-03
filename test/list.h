#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stdlib.h>
#include "mmu.h"
#include "errors.h"
typedef struct tListItem{
    void *data;
    struct tListItem *next;
}tListItem;

typedef struct{
    tListItem *first;
    tListItem *last;
    tListItem *act;
}tList;

void initList(tList*);
E_CODE listInsertFirst(tList*, void*);
E_CODE listInsertLast(tList*, void*);
E_CODE listDeleteFirst(tList*);
void Succ (tList*);
void postInsert (tList*, void*);
void Last (tList*);
#endif
