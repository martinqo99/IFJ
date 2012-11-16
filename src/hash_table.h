/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   hash_table.h
 * 
 * Popis:    
 * 
 * 
 * Datum:    22.10.2012
 * 
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */


#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

#include "mmu.h"
#include "strings.h"

typedef struct htableItem{
    struct htableItem* ptr;
    tString key;  
    
} *tHTableItemPtr, tHTableItem;

typedef struct{
    unsigned int size;
    tHTableItem** data;
    
} tHTable;

tHTable* htableCreate();
tHTableItem* htableItemCreate(tString);
void htableDestroy(tHTable*);
void htableItemDestroy(tHTableItem*);
void htableInit(tHTable*, unsigned int);
void htableDispose(tHTable*);

tHTableItemPtr htableLookup(tHTable*, tString);

unsigned int hash(tString, unsigned int);

#endif
