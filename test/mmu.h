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
#include <signal.h>
#include <assert.h>
#include "strings.h"
#define MMU_SIZE 32

typedef enum mmutableitemtype{
    MMU_UNDEFINED,
    MMU_MEMORY,
    MMU_FILE
} tMMUTableItemType;

typedef struct mmutableitem{
    //Klic datoveho bloku - adresa
    intptr_t key;
    tMMUTableItemType type;

    //Ukazatel na datovy blok
    void* ptr;
    unsigned long allocated;

    struct mmutableitem* next;
} *tMMUTableItemPtr, tMMUTableItem;

typedef struct mmutable{
    unsigned int size;
    tMMUTableItem** data;
} tMMUTable;

typedef struct mmu{
    tMMUTable* table;

    //Pocitadla
    unsigned long mallocs;
    unsigned long reallocs;
    unsigned long callocs;
    unsigned long fopens;
    unsigned long frees;
    unsigned long fcloses;

    unsigned long allocated;
} tMMU;

extern tMMU mmuTable;

//API pro pouzivani MMU
void mmuInit();
void* mmuMalloc(size_t);
void* mmuRealloc(void*, size_t);
void* mmuCalloc(size_t, size_t);
void* mmuFopen(const char*, const char*);
void mmuFree(void*);
void mmuFclose(void*);
void mmuGlobalFree();
//void mmuDump();

//Funkce pro praci s hashovaci tabulkou
tMMUTable* mmuTableCreate();
tMMUTableItem* mmuTableItemCreate(intptr_t);
void mmuTableDestroy(tMMUTable*);
void mmuTableItemDestroy(tMMUTableItem*);
void mmuTableInit(tMMUTable*, size_t);
void mmuTableDispose(tMMUTable*);
tMMUTableItem* mmuTableLookup(tMMUTable*, intptr_t);

//Hashovaci funkce
size_t hash(intptr_t, size_t);


#endif
