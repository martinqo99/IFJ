/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   main.c
 *
 * Popis:
 *
 *
 * Datum:    9.12.2012
 *
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "mmu.h"
#include "scanner.h"
#include "syntactic.h"
#include "interpret.h"

int main(int argc, char* argv[]){

    //Inicializace MMU
    mmuInit();

    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
        return ERROR_COMPILATOR;
    }

    if(!(gFileHandler = mmuFopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open input file\n");
        return ERROR_COMPILATOR;
    }

    initToken(&gToken);
    E_CODE err = ERROR_OK;

    tSymbolTable table;
    symbolTableInit(&table);

    err = parser(&table);

    if (err == ERROR_OK)
      err = interpret(&table);

    mmuFclose(gFileHandler);

    //mmuDump();

    mmuGlobalFree();

    return err;
}
