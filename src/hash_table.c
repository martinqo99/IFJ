/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   hash_table.c
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

#include "hash_table.h"

tHTable* htableCreate(){
    return (tHTable*)mmuMalloc(sizeof(tHTable));
}

tHTableItem* htableItemCreate(intptr_t key){
    tHTableItem* item = (tHTableItem*)mmuMalloc(sizeof(tHTableItem));
    
    if(!item)
        return NULL;
        
    item->ptr = NULL;
    item->next = NULL;    
    item->key = key;
    
    return item;
}

void htableDestroy(tHTable* T){
    if(!T)
        return;
    
    mmuFree(T);
}

void htableItemDestroy(tHTableItem* item){
    if(!item)
        return;

    mmuFree(item);
}

void htableInit(tHTable* T, size_t size){
    if(!T)
        return;
        
    T->data = mmuCalloc(size, sizeof(tHTableItemPtr));
    
    if(!T->data)
        return;
        
    T->size = size;
}

void htableDispose(tHTable* T){
    if(!T)
        return;
    
    if(T->data)
        mmuFree(T->data);
    
    T->data = NULL;
}

tHTableItemPtr htableLookup(tHTable* T, intptr_t key){
    if(!T)
        return NULL;
    
    size_t index = hash(key, T->size);
    
    tHTableItemPtr item = T->data[index];
    
    if(!item)
        return (T->data[index] = htableItemCreate(key));

    while(item){
        if(item->key == key)
            return item;

        if(!item->next)
            break;
        
        item = item->next;
    };
    
    tHTableItem* newItem = htableItemCreate(key);

    return (!newItem)? NULL : (item->next = newItem);
}

size_t hash(intptr_t key, size_t size){
    size_t sum = 0;
    
    printf("HASH %lu => ", key);
    while(key > 9){
        sum += key % 10;
        key /= 10;        
    }

    printf("%lu\n", (97*sum)%size);
    return (97 * sum)%size;
}
