/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   mmu.c (Memory management unit)
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

#include "mmu.h"

tMMU mmuTable;

void mmuInit(){
    //Inicializace pametovych pocitatel
    mmuTable.mallocs = 0;
    mmuTable.reallocs = 0;
    mmuTable.callocs = 0;
    mmuTable.frees = 0;
    mmuTable.allocated = 0;
    mmuTable.table = NULL;
    
    //Vytvoreni pametove tabulky
    mmuTable.table = htableCreate();
    
    if(!mmuTable.table){
        
        
    }
    
    //Inicializace pametove tabulky
    htableInit(mmuTable.table, MMU_SIZE);
}

void* mmuMalloc(size_t size){
    mmuTable.mallocs++;
    mmuTable.allocated += size;
    
    void* newPtr = malloc(size);
    
    
    if(!newPtr){
        
        
    }
    
    //tHTableItemPtr item = htableLookup(mmuTable.table, (intptr_t)newPtr);
    
    //if(!item)        
    //    return NULL;
    
    //item->allocated = size;
    //item->ptr = newPtr;
    
    return newPtr;
}

void* mmuRealloc(void* ptr, size_t size){
    mmuTable.reallocs++;
    mmuTable.allocated += size;
    
    void* newPtr = realloc(ptr, size);
    
    if(!newPtr){
        
        
    }
    
    return newPtr;
}

void* mmuCalloc(size_t num, size_t size){
    mmuTable.callocs++;
    mmuTable.allocated += size;
    
    void* newPtr = calloc(num, size);
    
    if(!newPtr){
        
        
    }
    
    return newPtr;
}

void mmuFree(void* ptr){
    mmuTable.frees++;
    
    free(ptr);
}

void mmuGlobalFree(){
    if(!mmuTable.table && !mmuTable.table->data)
        return;   
    
    tHTableItemPtr head = NULL;
    tHTableItemPtr item = NULL;

    //Pro kazdy mozny radek pametove tabulky
    for(unsigned int i = 0; i < mmuTable.table->size; i++){
        if(mmuTable.table->data[i] != 0){
            head = mmuTable.table->data[i];
            
            while((item = head)){
                head = head->next;
                //printf("GLOBAL FREE: [%d] %lu -> %lu\n", i, (intptr_t)item, (intptr_t)item->next);                
                
                if(item->allocated != 0)
                    mmuFree(item->ptr);
                
                mmuFree(item);
                
            }
        }        
    }
    
    //Uvolnime vnitrni pole pametove tabulky
    htableDispose(mmuTable.table);
    
    //Uvolnime pametovou tabulku
    htableDestroy(mmuTable.table);
    
    mmuTable.table = NULL;
    
    printf("\n\n--------------------- MMU report --------------------\n");
    printf("Mallocs: %lu | Callocs: %lu | Reallocs: %lu | Frees: %lu\n", mmuTable.mallocs, mmuTable.reallocs, mmuTable.callocs, mmuTable.frees);
    printf("Allocated: %lu bytes\n", mmuTable.allocated);
    printf("-----------------------------------------------------\n"); 
}

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

