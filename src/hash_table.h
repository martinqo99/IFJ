/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   hash_table.h
 * 
 * Popis:    
 * 
 * 
 * Datum:    21.10.2012
 * 
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */


#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

typedef struct htableItem{
    struct htableItem* ptr;
    char* uid;    
    
} *tHTableItemPtr, tHTable;

typedef struct{
    unsigned int size;
    tHTableItem** data;
    
} tHTable;

tHTable* htableCreate();
void htableDestroy(tHTable*);

void htableInit(tHTable*);
void htableDispose(tHTable*);

#endif
