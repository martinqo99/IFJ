/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   mmu.h (Memory management unit)
 * 
 * Popis:    
 * 
 * 
 * Datum:    28.11.2012
 * 
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include "strings.h"
#define MMU_SIZE 2

typedef struct htableitem{
    intptr_t key;  
    struct htableitem* next;
    
    void* ptr;
    unsigned long allocated;    
} *tHTableItemPtr, tHTableItem;

typedef struct thtable{
    unsigned int size;
    tHTableItem** data;    
} tHTable;

typedef struct mmu{
    tHTable* table;
    
    unsigned long mallocs;
    unsigned long reallocs;
    unsigned long callocs;
    unsigned long frees;
    
    unsigned long allocated;
} tMMU;

extern tMMU mmuTable;

void mmuInit();
void* mmuMalloc(size_t);
void* mmuRealloc(void*, size_t);
void* mmuCalloc(size_t, size_t);

void mmuFree(void*);
void mmuGlobalFree();

tHTable* htableCreate();
tHTableItem* htableItemCreate(intptr_t);
void htableDestroy(tHTable*);
void htableItemDestroy(tHTableItem*);
void htableInit(tHTable*, size_t);
void htableDispose(tHTable*);

tHTableItemPtr htableLookup(tHTable*, intptr_t);
size_t hash(intptr_t, size_t);

#endif
