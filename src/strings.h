/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    strings.h
 * Popis:     Tento soubor definuje co je to string
 * Datum:     17.10.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"

#define SIZE 20 // zacatecni velikost pole, pri realokaci se alokuje +SIZE
#define TRUE 1
#define FALSE 0
#define NEGATIVE -1

//Pozustatek...
typedef unsigned int uint;

typedef struct tstring {
  char *data;
  unsigned int lenght;
  unsigned int allocated;
} tString;

int strInit (tString *str);
//int strCmpRaw (tString *str, const char x);
tString strCreate (const char *str);
int strFree (tString *str);
int strClear (tString *str);
int strAdd (tString *str, char x);
int strCopy (tString *str, char *array);
int strCopyString (tString *strl, tString *strr);
int strCmp (tString *strl, tString *strr);
int strCmpRaw(tString *strl, const char* strr);
char *strRaw (tString *str);
uint strLen (tString *str);
uint strSize (tString *str);

#endif
