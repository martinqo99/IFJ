/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   lexikalny_analyzator.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#pragma once

/**
 * Stavy automatu
 */
typedef enum
{
    S_START,        // 00 - pociatocny stav
    S_KLUC,         // 01 - klucove slovo
    S_REZ,          // 02 - rezervovane slovo
    S_ENDOFFILE,    // 03 - EOF
    S_CHYBA,        // 04 - chyba
    S_KONEC,        // 05 - koncovy stav
    S_IDENT,        // 06 - indentifikator
    S_INT,          // 07 - integer
    S_DOUBLE,       // 08 - double
    S_DOUBLEP,      // 09 - pom double
    S_EXPO,         // 10 - exponential
    S_EXPOP,        // 11 - pom exp
    S_EXPOP2,       // 12 - pom2 exp
    S_STR,          // 13 - retazec
    S_PRIR,         // 14 - =
    S_STREDNIK,     // 15 - ;
    S_CARKA,        // 16 - ,
    S_BODKA,        // 17 - .
    S_LZ,           // 18 - (
    S_PZ,           // 19 - )
    S_ADD,          // 20 - +
    S_SUB,          // 21 - -
    S_MUL,          // 22 - *
    S_DIV,          // 23 - /
    S_MOC,          // 24 - ^
    S_TIL,          // 25 - ~
    S_VEC,          // 26 - >
    S_MEN,          // 27 - <
    S_VYKR,         // 28 - !
    S_STRP,	        // 29 - string
    S_DVOJB,        // 30 - ..
    S_POROV,        // 31 - ==
    S_NEROV,        // 32 - !=
    S_TILROV,       // 33 - ~=
    S_VECROV,       // 34 - >=
    S_MENROV,       // 35 - <=
    S_RCOM,         // 36 - --
    S_RCOMD,        // 37 - --x
    S_RCOMHZ,       // 38 - --[
    S_BCOMZ,        // 39 - --[[
    S_BCOMK,        // 40 - ]
} tStav;

/**
 * Struktura reprezentujuca token
 */
typedef struct
{
    tStav stav;     // stav lexemu
    char *data;     // hodnota lexemu
    int riadok;     // riadok lexemu
    int stlpec;     // stlpec lexemu
} tToken;

/**
 * Globalna premenna
 */
extern tToken token;

/**
 * Prototypy funkcii
 */
tToken getToken(void);  // vrati token

