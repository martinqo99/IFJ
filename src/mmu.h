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
#include "types.h"
#include "strings.h"
#include "hash_table.h"

#define MMU_SIZE 2

extern tMMU mmuTable;

void mmuInit();
void* mmuMalloc(size_t);
void* mmuRealloc(void*, size_t);
void* mmuCalloc(size_t, size_t);

void mmuFree(void*);
void mmuGlobalFree();

#endif
