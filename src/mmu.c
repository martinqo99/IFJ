/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   MMU.c (Memory management unit)
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
    mmuTable.mallocs = 0;
    mmuTable.reallocs = 0;
    mmuTable.callocs = 0;
    mmuTable.frees = 0;
    mmuTable.allocated = 0;
}

void* mmuMalloc(unsigned int size){
    mmuTable.mallocs++;
    mmuTable.allocated += size;
    
    void* newPtr = malloc(size);
    
    if(!newPtr){
        
        
    }
    
    return newPtr;
}

void* mmuRealloc(void* ptr, unsigned int size){
    mmuTable.reallocs++;
    mmuTable.allocated += size;
    
    void* newPtr = realloc(ptr, size);
    
    if(!newPtr){
        
        
    }
    
    return newPtr;
}

void* mmuCalloc(unsigned int num, unsigned int size){
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
    printf("\n\n--------------------- MMU report --------------------\n");
    printf("Mallocs: %u | Callocs: %u | Reallocs: %u | Frees: %u\n", mmuTable.mallocs, mmuTable.reallocs, mmuTable.callocs, mmuTable.frees);
    printf("Allocated: %u bytes\n", mmuTable.allocated);
    printf("-----------------------------------------------------\n");
}
