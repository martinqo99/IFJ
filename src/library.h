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

#include <stdarg.h>
#include "kmp.h"
#include "quicksort.h"

tSymbolData input (E_CODE *err);
tSymbolData numeric (tSymbolData id, E_CODE *err);
tSymbolData print (tSymbolData id1, ...);
tSymbolData typeOf (tSymbolData id);
tSymbolData len (tSymbolData id);
tSymbolData find (tSymbolData text, tSymbolData searched);
tSymbolData sort (tSymbolData nonsorted, E_CODE *err);

#endif
