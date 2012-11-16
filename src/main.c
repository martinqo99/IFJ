/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   main.c
 * 
 * Popis:    
 * 
 * 
 * Datum:    16.11.2012
 * 
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "errors.h"
#include "mmu.h"

FILE* fileHandler;

int main(int argc, char* argv[]){
    
    signal(SIGINT, mmuGlobalFree);
    
    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
        return ERROR_COMPILATOR;
    }
    
    if(!(fileHandler = fopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open input file\n");
        return ERROR_COMPILATOR;
    }
    
    
    
    fclose(fileHandler);
    
    mmuGlobalFree();
    
    return ERROR_OK;
}
