/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   syntakticky_analyzator.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */


#pragma once




#include "funkcie.h"
#include "lexikalny_analyzator.h"
#include "ial.h"
#include "vyrazy.h"
#include "tagenerator.h"

/**
 * Globalni promenna
 */
extern tSymbol name;
extern bool isWrite;
extern tBTSUzolPtr typPrirazeni;
extern tBTSUzolPtr volana_fce;
extern char * functionName;
extern TA_Item *ta;
extern int nazev_fce;
extern int *pole_back;
extern int index_pb;
extern bool isLocal;

extern tBTSUzolPtr cilovaAdresaFce;

/* 1=read;
 * 2=sort;
 *
 *
 * */
tError parser();
tError program();
tError funkce();
tError body();
tError params();
tError paramsNext();
tError varDeclar();
tError prirazeni();
tError list();
tError vest();
tError read();
tError write();
tError litExpr();
tError literal();
tError args();
tError argsNext();

/**
 * Funkce pro vyhledavani promennych ve stome
 */

void najdiKolize(tBTSUzolPtr uzel);
