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

/**
 * @info      Inicializace funkci pro praci s pameti
 * @return  void
 */
void mmuInit(){
    signal(SIGINT, mmuGlobalFree);
    signal(SIGABRT, mmuGlobalFree);

    //Inicializace pametovych pocitatel
    mmuTable.mallocs = 0;
    mmuTable.reallocs = 0;
    mmuTable.callocs = 0;
    mmuTable.fopens = 0;
    mmuTable.frees = 0;
    mmuTable.fcloses = 0;
    mmuTable.allocated = 0;
    mmuTable.table = NULL;

    //Vytvoreni pametove tabulky
    mmuTable.table = mmuTableCreate();

    assert(mmuTable.table);

    //Inicializace pametove tabulky
    mmuTableInit(mmuTable.table, MMU_SIZE);
}

/**
 * @info      Nahradni fce pro alokaci pameti - malloc
 * @param   size_t - pozadovana velikost pameti
 * @return  void* - ukazatel na naalokovane misto v pameti
 */
void* mmuMalloc(size_t size){
    mmuTable.mallocs++;
    mmuTable.allocated += size;

    void* newPtr = malloc(size);

    if (newPtr == NULL) {
      mmuGlobalFree();
      exit (ERROR_COMPILATOR);
    }

    assert(newPtr);

    tMMUTableItem* item = mmuTableLookup(mmuTable.table, (intptr_t)newPtr);

    assert(item);

    item->ptr = newPtr;
    item->allocated = size;
    item->type = MMU_MEMORY;

    return newPtr;
}

/**
 * @info      Nahradni fce pro realokaci pameti - realloc
 * @param   void* - ukazatel na puvodni kus pameti
 * @param   size_t - pozadovana velikost noveho kusu pameti
 * @return  void* - ukazatel na novy kus pameti
 */
void* mmuRealloc(void* ptr, size_t size){

    mmuTable.reallocs++;
    mmuTable.allocated += size;

    void* newPtr = realloc(ptr, size);

    if (newPtr == NULL) {
      mmuGlobalFree();
      exit (ERROR_COMPILATOR);
    }

    assert(newPtr);

    tMMUTableItem* item = NULL;

    if(newPtr != ptr){
        item = mmuTableLookup(mmuTable.table, (intptr_t)ptr);

        assert(item);

        item->allocated = 0;
        item->ptr = NULL;
    }

    item = mmuTableLookup(mmuTable.table, (intptr_t)newPtr);

    assert(item);

    item->ptr = newPtr;
    item->allocated = size;
    item->type = MMU_MEMORY;

    return newPtr;
}

/**
 * @info      Nahradni fce pro alokaci pameti - calloc
 * @param   size_t - pocet kousku pameti o velikosti podle druheho parametru
 * @return  void* - ukazatel na naalokovane misto v pameti
 */
void* mmuCalloc(size_t num, size_t size){
    mmuTable.callocs++;
    mmuTable.allocated += size;

    void* newPtr = calloc(num, size);

    if (newPtr == NULL) {
      mmuGlobalFree();
      exit (ERROR_COMPILATOR);
    }

    assert(newPtr);

    tMMUTableItem* item = mmuTableLookup(mmuTable.table, (intptr_t)newPtr);

    assert(item);

    item->ptr = newPtr;
    item->allocated = size;
    item->type = MMU_MEMORY;

    return newPtr;
}

/**
 * @info      Nahradni fce pro otevreni souboru
 * @param   const char* - jmeno souboru
 * @param   const char* - mod otevreni
 * @return  void* - ukazatel na otevreny soubor
 */
void* mmuFopen(const char* fileName, const char* mode){
    mmuTable.fopens++;
    mmuTable.allocated += sizeof(FILE*);

    void* newPtr = (FILE*)fopen(fileName, mode);

    if(!newPtr)
        return NULL;

    tMMUTableItem* item = mmuTableLookup(mmuTable.table, (intptr_t)newPtr);

    assert(item);

    item->ptr = newPtr;
    item->allocated = sizeof(FILE*);
    item->type = MMU_FILE;

    return newPtr;
}

/**
 * @info      Nahradni fce pro uvolneni pameti
 * @param   void* - ukazatel na misto v pameti, ktere chceme uvolnit
 * @return  void
 */
void mmuFree(void* ptr){
    mmuTable.frees++;

    if(!ptr)
        return;

    tMMUTableItem* item = mmuTableLookup(mmuTable.table, (intptr_t)ptr);

    if(!item)
        return;

    if(item->allocated != 0){
        free(ptr);
        item->allocated = 0;
        item->ptr = NULL;
    }
}

/**
 * @info      Nahradni fce pro uzavreni souboru
 * @param   void* - ukazatel na otevreny soubor
 * @return  void
 */
void mmuFclose(void* ptr){
    mmuTable.fcloses++;

    if(!ptr)
        return;

    tMMUTableItem* item = mmuTableLookup(mmuTable.table, (intptr_t)ptr);

    if(!item)
        return;

    if(item->allocated != 0){
        fclose(ptr);
        item->allocated = 0;
        item->ptr = NULL;
    }

}

/**
 * @info      Hlavni fce, ktera uklidi, co se neuklidilo rucne
 * @return  void
 */
void mmuGlobalFree(){
    if(!mmuTable.table && !mmuTable.table->data)
        return;

    tMMUTableItemPtr head = NULL;
    tMMUTableItemPtr item = NULL;

    //Pro kazdy mozny radek pametove tabulky
    for(unsigned int i = 0; i < mmuTable.table->size; i++){
        if(mmuTable.table->data[i] != 0){
            head = mmuTable.table->data[i];

            while((item = head)){
                head = head->next;
                //printf("GLOBAL FREE: [%d] %lu -> %lu\n", i, (intptr_t)item, (intptr_t)item->next);

                if(item->allocated != 0){
                    if(item->type == MMU_FILE)
                        mmuFclose(item->ptr);
                    else
                        mmuFree(item->ptr);
                }

                mmuTableItemDestroy(item);

                mmuTable.table->data[i] = head;
            }

            mmuTable.table->data[i] = NULL;
        }
    }

    //Uvolnime vnitrni pole pametove tabulky
    mmuTableDispose(mmuTable.table);

    //Uvolnime pametovou tabulku
    mmuTableDestroy(mmuTable.table);

    mmuTable.table = NULL;

    /*
    printf("\n\n---------------------------- MMU report ---------------------------\n");
    printf("Memmory (malloc/calloc/realloc/free): %lu/%lu/%lu/%lu\n", mmuTable.mallocs, mmuTable.callocs, mmuTable.reallocs, mmuTable.frees);
    printf("Files (open/close): %lu/%lu\n", mmuTable.fopens, mmuTable.fcloses);
    printf("Total allocated memory: %lu bytes\n", mmuTable.allocated);
    printf("-------------------------------------------------------------------\n");
    */
}

/*void mmuDump(){
    if(!mmuTable.table && !mmuTable.table->data)
        return;

    tMMUTableItemPtr item = NULL;

    printf("\n\n----------------------------- MMU dump ----------------------------\n");

    //Pro kazdy mozny radek pametove tabulky
    for(unsigned int i = 0; i < mmuTable.table->size; i++){
        if(mmuTable.table->data[i] != 0){
            printf("Row index: %u\n", i);
            item = mmuTable.table->data[i];

            while((item)){
                printf(" - Item %lu: type: %d | allocated: %lu | ptr: %lu | next: %lu\n", (intptr_t)item, item->type, item->allocated, (intptr_t)item->ptr, (intptr_t)item->next);

                item = item->next;
            }
        }
    }
    printf("-------------------------------------------------------------------\n");
}*/

/**
 * @info      Vytvoreni hashovaci tabulky pro MMU
 * @return  tMMUTable* - ukazatel na vytvorenou hashovaci tabulku
 */
tMMUTable* mmuTableCreate(){
    return (tMMUTable*)malloc(sizeof(tMMUTable));
}

/**
 * @info      Vytvoreni noveho prvku v hashovaci tabulce
 * @param   intptr_t - nove vkladany klic
 * @return  tMMUTableItem* - ukazatel na novou polozku v hashovaci tabulce
 */
tMMUTableItem* mmuTableItemCreate(intptr_t key){
    tMMUTableItem* item = (tMMUTableItem*)malloc(sizeof(tMMUTableItem));

    if(!item)
        return NULL;

    item->ptr = NULL;
    item->next = NULL;
    item->key = key;
    item->type = MMU_UNDEFINED;

    return item;
}

/**
 * @info      Smazani tabulky
 * @param   tMMUTable* - ukazatel na hashovaci tabulku
 * @return  void
 */
void mmuTableDestroy(tMMUTable* T){
    if(!T)
        return;

    free(T);
}

/**
 * @info      Smazani prvku v hashovaci tabulce
 * @param   tMMUTableItem* - ukazatel na polozku v hashovaci tabulce
 * @return  void
 */
void mmuTableItemDestroy(tMMUTableItem* item){
    if(!item)
        return;

    free(item);
}

/**
 * @info      Inicializace hashovaci tabulky
 * @param   tMMUTable* - ukazatel na hashovaci tabulku
 * @param   size_t - velikost alokovane pameti
 * @return  void
 */
void mmuTableInit(tMMUTable* T, size_t size){
    if(!T)
        return;

    T->data = calloc(size, sizeof(tMMUTableItemPtr));

    if(!T->data)
        return;

    T->size = size;
}

/**
 * @info      Smazani tabulky
 * @param   tMMUTable* - ukazatel na hashovaci tabulku
 * @return  void
 */
void mmuTableDispose(tMMUTable* T){
    if(!T)
        return;

    if(T->data)
        free(T->data);

    T->data = NULL;
}

/**
 * @info      Vyhledavani v hashovaci tabulce
 * @param   tMMUTable* - ukazatel na hashovaci tabulku
 * @param   intptr_t - hledany klic
 * @return  tMMUTableItem* - ukazatel na hledany prvek nebo NULL
 */
tMMUTableItem* mmuTableLookup(tMMUTable* T, intptr_t key){
    if(!T)
        return NULL;

    size_t index = hash(key, T->size);

    tMMUTableItem* item = T->data[index];

    if(!item){
        T->data[index] = mmuTableItemCreate(key);

        return (T->data[index]);
    }

    while(item){
        if(item->key == key)
            return item;

        if(!item->next)
            break;

        item = item->next;
    };

    tMMUTableItem* newItem = mmuTableItemCreate(key);

    return (!newItem)? NULL : (item->next = newItem);
}

/**
 * @info      Hashovaci funkce
 * @param   intptr_t - klic
 * @param   size_t - velikost hashovaci tabulky
 * @return  size_t - radek v hashovaci tabulce
 */
size_t hash(intptr_t key, size_t size){
    size_t sum = 0;

    //printf("HASH %lu => ", key);
    while(key > 9){
        sum += key % 10;
        key /= 10;
    }

    //printf("%lu\n", (97*sum)%size);
    return (97 * sum)%size;
}
