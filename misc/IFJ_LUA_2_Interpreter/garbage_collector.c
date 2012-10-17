/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   garbage_collector.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#include <stdlib.h>
#include <stdio.h>

#include "garbage_collector.h"
#include "funkcie.h"

tGarbageRecordPtr garbageHead = NULL;
tGarbageRecordPtr garbageLast = NULL;

void * advMalloc(int velikost)
{
    tGarbageRecordPtr tmpNode = malloc(sizeof(struct GarbageRecord));
    if(tmpNode == NULL) {
        error = EINTERN;
        return NULL;
    }
    tmpNode->data = NULL;
    tmpNode->nextPtr = NULL;
    tmpNode->velikost = velikost;

    if(garbageHead == NULL)
        garbageHead = tmpNode;


    if(garbageLast != NULL)
    {
        garbageLast->nextPtr = tmpNode; //navazu predposledni
        garbageLast = tmpNode; //posunu posledni
    }
    else garbageLast = tmpNode;

    void * tmpAllocated = (void *)malloc(velikost);
    if(tmpAllocated == NULL) {
        error = EINTERN;
        return NULL;
    }
    tmpNode->data = tmpAllocated; //vlozim ukazatel do stromu

    return tmpAllocated;
}

void globalFree()
{
    tGarbageRecordPtr tmpVar = NULL;
    tmpVar = garbageHead;
    while(tmpVar != NULL) //uvolnim data programu
    {
        if(tmpVar->data != NULL)
            free(tmpVar->data);
        tmpVar = tmpVar->nextPtr;
    }
    tmpVar = garbageHead;
    while(tmpVar != NULL)//uvolnim samotny list
    {
        garbageHead = tmpVar;
        tmpVar = tmpVar->nextPtr;
        free(garbageHead);
    }
}

void * advRealloc(void * destinationAdr, int velikost)
{
    void * tmpVar = NULL;
    if(destinationAdr == NULL)
    {
        destinationAdr = advMalloc(velikost);
        return destinationAdr;
    }
    else
    {
        tmpVar = realloc(destinationAdr, velikost);
        if(tmpVar != destinationAdr)
            advAddReallocMem(tmpVar, velikost, destinationAdr);
        return tmpVar;
    }
}

void advAddReallocMem(void * tmpVar, int velikost, void * toNULL)
{
    advFindAndNULL(toNULL); //nepouzivana adresa se da na NULL

    tGarbageRecordPtr tmpNode = malloc(sizeof(struct GarbageRecord));
    if(tmpNode == NULL) {
        error = EINTERN;
        return ;
    }

    tmpNode->data = tmpVar;
    tmpNode->nextPtr = NULL;
    tmpNode->velikost = velikost;

    if(garbageHead == NULL)
        garbageHead = tmpNode;


    if(garbageLast != NULL)
    {
        garbageLast->nextPtr = tmpNode; //navazu predposledni
        garbageLast = tmpNode; //posunu posledni
    }
    else garbageLast = tmpNode;

    //tmpNode->data = tmpVar; //vlozim ukazatel do stromu
}

void advFindAndNULL(void * toNULL)
{
    tGarbageRecordPtr tmp = garbageHead; //skocim na hlavicku
    if(tmp == NULL)
        return ;

    while(tmp != NULL)
    {
        if(tmp->data == toNULL)
            tmp->data = NULL;
        tmp = tmp->nextPtr;
    }
}
