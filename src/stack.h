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

#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include "MMU.h"

#define STACK_EOK      0
#define STACK_ERR    -10
#define STACK_EMPTY  -11
#define STACK_MEMORY -12

typedef struct stackPtr{
    struct stackPtr *ptr;
    void* data;

} *tStackPtr;

typedef struct{
    unsigned int size;
    tStackPtr top;

} tStack;

void stackInit(tStack*);
void stackFree(tStack*);
void stackPush(tStack*, void*);
void* stackTop(tStack*);
void* stackPop(tStack*);

bool stackEmpty(tStack*);
unsigned int stackSize(tStack*);

#endif
