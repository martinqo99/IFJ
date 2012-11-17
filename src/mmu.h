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
#include "strings.h"
//#include "hash_table.h"

typedef struct MMU{
	void* ptr;
	
} tMMU;


void mmuInit();
void* mmuMalloc(unsigned int);
void* mmuRealloc(void*, unsigned int);
void* mmuCalloc(unsigned int, unsigned int);

void mmuFree(void*);
void mmuGlobalFree();

#endif
