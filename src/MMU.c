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

#include "MMU.h"

void* mmuMalloc(unsigned int size){
	return malloc(size);
}

void* mmuRealloc(void* ptr, unsigned int size){
	return realloc(ptr, size);
}

void* mmuCalloc(unsigned int num, unsigned int size){
	return calloc(num, size);
}

void mmuFree(void* ptr){
	free(ptr);
}

void mmuGlobalFree(){
	
}
