/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   advanced_strings.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#include "advanced_strings.h"
#include "garbage_collector.h"
#include <string.h>

#define OK_STR 0
#define ERR_STR 1
#define ALLOC_LEN 10

int strCreate(string* str)
{
    str->delka = 0;
    str->data = (char *)advMalloc(sizeof(ALLOC_LEN));
    if(str->data == NULL)
        return ERR_STR;

    str->alokovano = ALLOC_LEN;
    str->data[0] = '\0';
    return OK_STR;
}

int strCreateAdv(string* str, int velikost)
{
    str->delka = 0;
    str->data = (char *)advMalloc(sizeof(velikost));
    if(str->data == NULL)
        return ERR_STR;

    str->alokovano = velikost;
    str->data[0] = '\0';
    return OK_STR;
}

void strfree(string* str)
{
    //free(str->data);
    str->alokovano = 0;
    str->delka = -1;
}

void strClean(string* str)
{
    str->delka = 0;
    str->data[0] = '\0';
}

int strToDefaultState(string* str)
{
    //free(str->data);
    if (strCreate(str) == ERR_STR);
    return ERR_STR;

    return OK_STR;
}

int strAppendChar(string* str, char pis)
{
    if((str->delka + 1)  < str->alokovano)
    {
        str->data[str->delka] = pis;
        str->data[++str->delka] = '\0';
        return OK_STR;
    }
    else
    {
        str->data = (char *)advRealloc(str->data, str->delka + ALLOC_LEN);
        if(str->data == NULL)
            return ERR_STR;
        str->alokovano = str->alokovano + ALLOC_LEN;
        str->data[str->delka] = pis;
        str->data[++str->delka] = '\0';
        return OK_STR;
    }
}

char * strAppendString(string* str1, string* str2)
{
    if(str1->data != NULL)
        str1->delka = strlen(str1->data);
    else str1->delka = 0;

    if(str2->data != NULL)
        str2->delka = strlen(str2->data);
    else str2->delka = 0;

    char * tmpVar = NULL;
    tmpVar = (char*)advMalloc(sizeof(char)*(str1->delka + str2->delka + 1));

    int delka = str1->delka + str2->delka + 1;
    int k = 0;
    while (k < delka)
    {
        tmpVar[k] = '\0';
        k++;
    }

    if(str1->data != NULL)
        memcpy ( tmpVar, str1->data, str1->delka );
    int i = str1->delka;
    int j = 0;
    if(str2->data != NULL)
    {
        while (i < (str1->delka + str2->delka + 1) )
        {
            tmpVar[i] = str2->data[j];
            i++;
            j++;
        }
    }
    return tmpVar;
}

int strCopy(string* str1, string* str2)
{
    if (str2->delka >= str1->alokovano)
    {
        str1->data = (char*) advRealloc(str1->data, str2->delka + 1);
        if (str1->data == NULL)
            return ERR_STR;
        str1->alokovano = str2->delka + 1;
    }
    str1->delka = str2->delka;
    strcpy(str1->data, str2->data);
    return OK_STR;
}

int strAdvCmp(string* str1, string* str2)
{
    return strcmp(str1->data, str2->data);
}

int strCmpStrVsConst(string* str1, char* str2)
{
    return strcmp(str1->data, str2);
}

char* strGetData(string *str)
{
    return str->data;
}

int strGetLength(string* str)
{
    return str->delka;
}

int strGetfreeSpace(string* str)
{
    return str->alokovano - str->delka -1;
}

int strGetDataSize(string* str)
{
    return str->alokovano;
}
