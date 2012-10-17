/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   funkcie.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

/**
 * Hlavickove subory
 */
#include "funkcie.h"
#include "lexikalny_analyzator.h"
#include "garbage_collector.h"

/**
 * Globalne premenne
 */
tError error = EOK;
int riadok = 1;
int stlpec = 1;

/**
 * Nazvy jednotlivych chyb
 */
const char *tError_mena[] =
{
    "",
    "Chyba v ramci lexikalnej analyzy\0",
    "Chyba v ramci syntaktickej analyzy\0",
    "Chyba v ramci semantickej analyzy\0",
    "Chyba interpretace\0",
    "Interna chyba prekladaca\0"
};

/**
 * Vypise chybu
 */
void perr(void)
{
    fprintf(stderr, "%s radek:%d sloupec %d\n", tError_mena[error], token.riadok,token.stlpec);
}

void perrdet()
{
    fprintf(stderr, "%s na riadku %3d stlpci %3d\n", tError_mena[error], riadok, stlpec);
}
