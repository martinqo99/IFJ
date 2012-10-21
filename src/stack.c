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

void stackInit(tStack* S){
	if(!S)
		return;
		
	S->top = NULL;
	S->size = 0;
}

void stackFree(tStack* S){
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

void* stackTop(tStack* S){
	return (!S || S->top)? NULL : S->top->data;	
}

void* stackPop(tStack* S){
	if(!S || !S->top)
		return NULL;
		
	tStackPtr item = S->top;
	void* data = item->data;
	
	S->top = item->ptr;
	
	mmuFree(item);
	
	return data;
}

bool stackEmpty(tStack* S){
	if(!S)
		return false;	
	
	return (S->top)? true : false;
}

unsigned int stackSize(tStack* S){
	return (S)? S->size : 0;	
}
