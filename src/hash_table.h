/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   hash_table.h
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


#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

#include <stdint.h>
#include "types.h"
#include "strings.h"
#include "mmu.h"

tHTable* htableCreate();
tHTableItem* htableItemCreate(intptr_t);
void htableDestroy(tHTable*);
void htableItemDestroy(tHTableItem*);
void htableInit(tHTable*, size_t);
void htableDispose(tHTable*);

tHTableItemPtr htableLookup(tHTable*, intptr_t);
size_t hash(intptr_t, size_t);

#endif
