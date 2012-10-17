/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   vyrazy.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#pragma once

/**
 * Hlavickove soubory
 */
#include "ial.h"
#include "lexikalny_analyzator.h"
#include "funkcie.h"
#include "syntakticky_analyzator.h"
#include "tagenerator.h"
#include <stdlib.h>

//globalni promenna pro generovani unikatnich nazvu konstant
extern unsigned int nameID;
extern bool isExprWr;

/**
 * enumy pro vyplneni staticke precedencni tabulky
 */

typedef enum
{
    M,	//< mensi
    V,	//> vetsi
    R,	//= rovno
    E	//prazdne misto v tabulce -> synt. chyba
} tPrvkyindexu;

typedef enum
{
    PLUS,		//0 +
    MINUS,	//1 -
    KRAT,		//2 *
    DELENO,	//3 /
    MOCNINA,	//4 ^
    KONKAT,	//5 ..
    ROVNITKO,	//6 ==
    NEROVNITKO,	//7 !=
    MENSIROVNO,	//8 <=
    VETSIROVNO,	//9 >=
    VICE,		//10 >
    MENE,		//11 <
    LZAVORKA,	//12 (
    PZAVORKA,	//13 )
    ID,		//14 promenna a konstanta
    FCE,		//15 volani funkce
    CARKA,	//16 ,
    KDOLAR,	//17 $ konec vyrazu
    MENSITKO,	//18 oznaceni < na zasobniku
    NETERM,	//19 neterminalni znak napr. E->
    CHYBA		//20 jiny token do vyrazu nepripoustime
} tIndexy;

/**
 * zasobnik pro vyrazy
 */
//datova slozka na zasobniku se sklada z vicero prvku
typedef struct
{
    tSymbol data;		//polozka tabulky symbolu
    tIndexy p;	//pseudopolozky s kterymi budu operovat na zasobniku
} tData;

extern tData neterm; //globalni pro ukladani vysledku exp

typedef struct tZasPrvek
{
    struct tZasPrvek *ptr;
    tData item;
} *tZPrvekptr;

typedef struct
{
    tZPrvekptr vrchol;
} tVZasobnik;

/**
 * Prototypy funkci
 */
//pparser
tError pparser();	//main precedencni analyzy
// zasobnik
void VSinit(tVZasobnik *zasobnik);	//inicializace zasobniku
void VSpush(tVZasobnik *zasobnik, tData item);	//push
void VSpop(tVZasobnik *zasobnik);				//pop
void VStop(tVZasobnik *zasobnik, tData *item);
void VStoppop(tVZasobnik *zasobnik, tData *item);
bool VSempty(tVZasobnik *zasobnik);
