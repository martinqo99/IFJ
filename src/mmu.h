/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   MMU.h (Memory management unit)
 * 
 * Popis:    
 * 
 * 
 * Datum:    18.10.2012
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
#include "types.h"
#include "strings.h"
#include "hash_table.h"

typedef struct mmuitem{
    void* ptr;

    unsigned int allocated;
} tMMUItem;

typedef struct mmu{
    tHTable table;
    
    unsigned int mallocs;
    unsigned int reallocs;
    unsigned int callocs;
    unsigned int frees;
    
    unsigned int allocated;
} tMMU;

extern tMMU mmuTable;

void mmuInit();
void* mmuMalloc(unsigned int);
void* mmuRealloc(void*, unsigned int);
void* mmuCalloc(unsigned int, unsigned int);

void mmuFree(void*);
void mmuGlobalFree();

#endif
