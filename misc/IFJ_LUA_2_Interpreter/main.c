/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   main.c
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
#include "garbage_collector.h"
#include "ial.h"
#include "syntakticky_analyzator.h"
#include "clean.h"
#include <signal.h>


tFile subor;


/**
 * Inicializujeme prekladac
 */
static void initMain(int argc, char **argv)
{
    // overime argumenty
    if (argc != 2)
    {
        error = EINTERN;
        fprintf(stderr, "Byl zadan spatny pocet parametru!\n");
        return;
    }

    // pokusime se otevrit soubor
    if (!(subor = fopen(argv[1], "r")))
    {
        error = EINTERN;
        fprintf(stderr, "Nepodarilo se otevrit soubor!\n");
        return;
    }

    // inicializujeme tabulku symbolov
    TSinit();


}

/**
 * Zacatek programu
 */
int main(int argc, char **argv)
{
    (void) signal(SIGINT,clean_all);
    //(void) signal(SIGTERM,clean_all);
    //(void) signal(SIGQUIT,clean_all);

    initMain(argc, argv);
    if (error)
    {
        perr();
        return error;
    }

    ////////////////////////////////////////
    ////      SYNTAKTICKA ANALYZA       ////
    ////////////////////////////////////////
    error = parser();
    if (error == ELEX)
    {
        perrdet();
        return error;
    }
    else if (error)
    {
        perr();
        return error;
    }

    // zrusime tabulku symbolov
    TSdispose();

    //zrusime tabulku 3add kodu


    // zavrieme subor
    fclose(subor);
    globalFree();

    return EOK;
}
