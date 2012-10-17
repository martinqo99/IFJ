/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   funkcie.h
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
#include <stdbool.h>
#include <string.h>

/**
 * Typ pre ukazatel na subor
 */
typedef FILE *tFile;

/**
 * Chybove kody.
 */ 
typedef enum Error
{
    EOK,        // 0 - vsetko ok
    ELEX,       // 1 - chyby v ramci lexikalnej analyzy
    ESYN,       // 2 - chyby v ramci syntaktickej analyzy
    ESEM,       // 3 - chyby v ramci semantickej analyzy
    EINT,       // 4 - chyby interpretace
    EINTERN,    // 5 - interne chyby interpretu
} tError;
 
/**
 * Globalne premenne
 */
tFile subor;
tError error;
int riadok;
int stlpec;
 
/**
 * Prototypy funkcii
 */
void perr(void);
void perrdet(void);
