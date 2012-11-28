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
#include "scanner.h"

int main(int argc, char* argv[]){
    
    signal(SIGINT, mmuGlobalFree);
    
    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
        return ERROR_COMPILATOR;
    }
    
    if(!(gFileHandler = fopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open input file\n");
        return ERROR_COMPILATOR;
    }
 
    mmuInit();   
    
    /*
      Test area
    */
    
    //initToken(&gToken);
    //printf("Row: %d\nColumn: %d\n", gToken.row, gToken.column);
    //getToken(&gToken);
    
    fclose(gFileHandler);
    
    mmuGlobalFree();
    
    return ERROR_OK;
}
