/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    library.h
 * Popis:     Vestavene funkce
 * Datum:     3.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

#include <strarg.h>
#include "kmp.h"

typedef enum tdatatype{
  DT_NIL,
  DT_BOOL,
  DT_NUMBER,
  DT_STRING,
  DT_UNKNOWN
} tDataType;

typedef union tdata{
  int iData;
  double dData;
  tString sData;
} tData;

typedef struct tlibrarydata {
  tDataType type;
  tData data;
} tLibraryData;

tString input();
double numeric (tLibraryData id);
tKeyword print (tLibraryData id1, ...);
double typeOf (tLibraryData id);
double len (tLibraryData id);
double find (tString text, tString searched);
tString sort (tString nonsorted);

#endif
