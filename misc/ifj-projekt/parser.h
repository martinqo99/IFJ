/*
 * @description   Syntakticka analyza shora dolu (rekurzivnim sestupem)
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stdlib.h>

#include "table.h"
#include "scanner.h"
#include "str.h"
#include "list.h"
#include "expression.h"

#define PRS_OK    0
#define LEX_ERR  -1
#define SYN_ERR  -2
#define SEM_ERR  -3
#define RUN_ERR  -4
#define INTR_ERR -5

string  attr;
int     token;
TTable *table;
TList  *instr; // zde je ulozen seznam instrukci naposled vlozene funkce

/*
 * Syntakticka analyza shora dolu rekurzivnim setupem
 * @param   Tabulka funkci
 * @return  vyse uvedena makra
 */
int parser(TTable*);

#endif // PARSER_H_INCLUDED
