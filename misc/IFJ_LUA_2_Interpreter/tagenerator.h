/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   tagenerator.h
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

#pragma once

#include "ial.h"
#include "funkcie.h"
#include "lexikalny_analyzator.h"



/*****
 * Struktura do ktere se ukladaji instrukce s informacemi o operatorech, vysledku, poradi operace a pripadnem skoku
 *****/
typedef struct
{
    size_t operation;
    void *op1;
    void *op2;
    void *result;
    int jump;
    int end;
} TA_Item;

/*****
 * konstatna po kolika se budou prialokovavat polozky do pole 3adresnych instrukci nebo pole jumpu
 *****/
#define ALL_MORE 10

/*****
 * vycet instrukci
 *****/

enum op
{
    /** numericke operace **/
    I_ADD = 0,	//+ nebo konkatenace retezcu
    I_SUB,		//-
    I_DIV,		// /
    I_MUL,      // *
    I_POW,		// ^
    I_ASSIGN,	// =
    /** logicke operace **/
    I_EQUAL,		// ==
    I_NEQUAL,	// !=
    I_ELESS,		// <=
    I_LESS,		// <
    I_MORE,		// >
    I_EMORE,		// >=
    /** vestavene fce **/
    I_READ,
    I_WRITE,
    I_SORT,
    I_FIND,
    I_SUBS,
    I_TYPE,

    /** jumpy **/
    I_LABEL,		// jenom labeldsfds
    I_TJUMP,		//true jump
    I_FJUMP,		//false jump
    I_JUMP,		//nepodmineny jump
    I_GOTO,
    I_FCEJUMP,

};

/*****
 * Hlavicky funkci
 *****/

void ta_Initialize(TA_Item **ta_table);
void ta_Insert(TA_Item **ta_table, size_t operation, tBTSUzolPtr op1, tBTSUzolPtr op2, tBTSUzolPtr result);
void ta_InsertJump(TA_Item **ta_table, size_t operation, tBTSUzolPtr op1, int jump);
void ta_Get(TA_Item **ta_table, size_t *operation, tBTSUzolPtr *op1, tBTSUzolPtr *op2, tBTSUzolPtr *result, int position);
void ta_Destroy(TA_Item **ta_table);
