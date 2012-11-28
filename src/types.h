/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   types.h
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

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>

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

#endif