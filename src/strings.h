/*
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka IFJ12
 * Subor:   strings.h
 * Datum:   17.10.2012
 * Autori:  Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *          Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *          Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *          Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *          Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tstring {
  char *data;
  uint lenght;
  uint allocated;
} tString;

int strInit (tString *str);
int strFree (tString *str);
int strClear (tString *str);
int strAdd (tString *str, char x);
char *strCopy (tString *str); // chci upresneni
int strCmp (tString *strl, tString *strr);
char *strRaw (tString *str);
int strLen (tString *str);
int strSize (tString *str);