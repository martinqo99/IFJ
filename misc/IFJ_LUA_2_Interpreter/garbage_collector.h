/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   garbage_collector.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#pragma once

typedef struct GarbageRecord
{
    void * data;
    int velikost;
    void * nextPtr;
} tGarbageRecord, *tGarbageRecordPtr;




void * advMalloc(int velikost);
void * advRealloc(void * destinationAdr, int velikost);
void  globalFree();

void advAddReallocMem(void * tmpVar, int velikost, void * toNULL);
void advFindAndNULL(void * toNULL);
