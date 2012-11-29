/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   stack.h
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

#include "stack.h"

/**
 * @info      Vytvoreni zasobniku
 * @return  tStack* - ukazatel na nove vytvoreny zasobnik
 */
tStack* stackCreate(){
    return (tStack*)mmuMalloc(sizeof(tStack));
}

/**
 * @info      Smazani zasobniku
 * @param   tStack* - ukazatel na zasobnik
 * @return  void
 */
void stackDestroy(tStack* S){
    if(!S)
        return;

    mmuFree(S);
}

/**
 * @info      Inicializace zasobniku
 * @param   tStack* - ukazatel na zasobnik
 * @return  void
 */
void stackInit(tStack* S){
    if(!S)
        return;

    S->top = NULL;
    S->size = 0;
}

/**
 * @info      Smazani zasobniku
 * @param   tStack* - ukazatel na zasobnik
 * @return  void
 */
void stackDispose(tStack* S){
    if(!S)
        return;

    tStackPtr iterator = NULL;

    while((iterator = S->top)){
        S->top = iterator->ptr;

        mmuFree(iterator);
    }

    S->top = NULL;
    S->size = 0;
}

/**
 * @info      Vlozeni hodnoty na zasobnik
 * @param   tStack* - ukazatel na zasobnik
 * @param   void* - ukazatel na vkladana data
 * @return  void
 */
void stackPush(tStack* S, void* data){
    if(!S)
        return;

    tStackPtr item = mmuMalloc(sizeof(struct stackPtr));

    if(!item)
        return;

    item->data = data;
    item->ptr = NULL;

    if(S->top)
        item->ptr = S->top;

    S->top = item;
}

/**
 * @info      Vrati data z vrcholu zasobniku
 * @param   tStack* - ukazatel na zasobnik
 * @return  void* - ukazatel na data
 */
void* stackTop(tStack* S){
    return (!S || !S->top)? NULL : S->top->data;
}

/**
 * @info      Vrati data z vrcholu zasobniku a zbavi se jich
 * @param   tStack* - ukazatel na zasobnik
 * @return  void* - ukazatel na data na vrcholu zasobniku
 */
void* stackPop(tStack* S){
    if(!S || !S->top)
        return NULL;

    tStackPtr item = S->top;
    void* data = item->data;

    S->top = item->ptr;

    mmuFree(item);

    return data;
}

/**
 * @info      Urceni, jestli je stack prazdny
 * @param   tStack* - ukazatel na zasobnik
 * @return  true || false
 */
bool stackEmpty(tStack* S){
    if(!S)
        return true;

    return (!S->top)? true : false;
}

/**
 * @info      vrati velikost zasobniku
 * @param   tStack* - ukazatel na zasobnik
 * @return  unsigned int - velikost zasobniku
 */
unsigned int stackSize(tStack* S){
    return (S)? S->size : 0;
}
