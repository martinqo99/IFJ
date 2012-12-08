/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    kmp.h
 * Popis:     Hlavickovy soubor pro hledani podretezce v retezci
 * Datum:     17.11.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef KMP_H_INCLUDED
#define KMP_H_INCLUDED

#include "mmu.h"
#include "strings.h"

void kmpCrtTable (tString text, int table[]);
int kmpSearch (tString text, tString searched);

#endif
