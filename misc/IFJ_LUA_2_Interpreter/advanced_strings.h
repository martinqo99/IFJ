/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   advanced_strings.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "funkcie.h"

typedef struct
{
    int delka;
    int alokovano;
    char *data;
} string;

int strCreate(string *str);
int strCreateAdv(string *str, int velikost);
void strFree(string *str);

void strClean(string *str);
int strToDefaultState(string *str);
int strAppendChar(string *str, char pis);
char* strAppendString(string *str1, string *str2);
int strCopy(string *str1, string *str2);
int strAdvCmp(string *str1, string *str2);
int strCmpStrVsConst(string *str1, char *str2);

char* strGetData(string *str);
int strGetLength(string *str);
int strGetFreeSpace(string *str);
int strGetDataSize(string *str);
