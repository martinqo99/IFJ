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

tLibraryData input (E_CODE *err);
tLibraryData numeric (tLibraryData id, E_CODE *err);
tLibraryData print (tLibraryData id1, ...);
tLibraryData typeOf (tLibraryData id);
tLibraryData len (tLibraryData id);
tLibraryData find (tLibraryData text, tLibraryData searched);
tLibraryData sort (tLibraryData nonsorted);

#endif
