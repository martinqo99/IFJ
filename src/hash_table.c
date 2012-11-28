/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   hash_table.c
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

#include "hash_table.h"

tHTable* htableCreate(){
    return (tHTable*)mmuMalloc(sizeof(tHTable));
}

tHTableItem* htableItemCreate(tString key){
    tHTableItem* item = (tHTableItem*)mmuMalloc(sizeof(tHTableItem));
    
    if(!item)
        return NULL;
        
    item->ptr = NULL;
    
    strCopyString(&(item->key), &key);
    
    return item;
}

void htableDestroy(tHTable* T){
    if(!T)
        return;
    
    free(T);
}

void htableItemDestroy(tHTableItem* item){
    if(!item)
        return;

    free(item);
}

void htableInit(tHTable* T, unsigned int size){
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
        free(T->data);
    
    T->data = NULL;
}

tHTableItemPtr htableLookup(tHTable* T, tString key){
    if(!T)
        return NULL;
    
    unsigned int index = hash(key, T->size);
    
    tHTableItemPtr item = T->data[index];
    
    if(!item)
        return (T->data[index] = htableItemCreate(key));
    
    do{
        if(strCmp(&(item->key), &key))
            return item;
        
        item = item->ptr;
    }while(item->ptr);

    tHTableItem* newItem = htableItemCreate(key);
    
    return (!newItem)? NULL : (item->ptr = newItem);
}

unsigned int hash(tString key, unsigned int size){
    unsigned int sum = 0;
    
    char* rawKey = key.data;
    
    while(*rawKey != '\0'){
        sum += (int)*rawKey; 
        
        rawKey++;
    }
    
    return (97 * sum)%size;
}
