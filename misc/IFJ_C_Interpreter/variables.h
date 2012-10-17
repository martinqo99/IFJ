/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: variables.h - modul tabulky konstant a promenych
 * Autor: David Barina, xbarin02
 */

#ifndef VARIABLES_H
#define VARIABLES_H

#include "scanner.h"

/***
 * TVarType
 *   typ promene / konstatny pro tabulky konstant a promenych 
 */
typedef enum
        {
          vtInt,     // promena / konstanta typu Int
          vtFloat,   // typu Float (double)
          vtString,  // String (char *)
          vtUninit   // zatim neinicializovana (!)
        } TVarType;

/***
 * TVariable
 *   typ konstanty / promene pro tabulky konstant a promenych
 *   nepracujte s ni primo, ale pres urcene funkce!
 *   linearni seznam
 */
typedef struct _TVariable
        TVariable, *PVariable;

/***
 * TConstantTable
 *   vlastni tabulka konstant, vkladani nakonec
 */
typedef struct
        {
          PVariable first;  // ukazatel na prvni prvek (NULL pri zadnem)
        } TConstantTable, *PConstantTable;

/***
 * TVariableTable
 *   vlastni tabulka promenych, vkladani nazacatek
 */
typedef struct
        {
          PVariable first;  // ukazatel na prvni prvek (NULL pri zadnem)
        } TVariableTable, *PVariableTable;

/***
 * ConstantsCreate()
 *   vytvori tabulku konstant a vrati jeji adresu 
 */
PConstantTable ConstantsCreate();

/***
 * ConstantsInsert()
 *   vlozi prvek do tabulky konstant a vrati ukazatel na nej
 *   1. parametr - tabulka konstant 
 *   2. parametr - typ konstanty
 *   3. parametr - adresa konstanty   
 */
PVariable ConstantsInsert(PConstantTable table, TVarType type, void *value);

/***
 * ConstantsDestroy()
 *   zrusi tabulku konstant i s obsahem (vsechny konstanty)
 */
void ConstantsDestroy(PConstantTable table);

/***
 * VariablesCreate()
 *   vytvori tabulku promenych a vrati jeji adresu 
 */
PVariableTable VariablesCreate();

/***
 * VariablesInsert()
 *   vlozi prvek do tabulky promenych a vrati ukazatel na nej
 *   promena je napocatku neinicializovana 
 *   parametr - tabulka promenych
 */
PVariable VariablesInsert(PVariableTable table);

/***
 * VariablesStore()
 *   funkce store (pro runtime), kt. ulozi do promene hodnotu
 *   1. parametr - promena
 *   2. parametr - typ noveho obsahu
 *   3. parametr - hodnota noveho obsahu (srara se uvolni)
 */
void VariablesStore(PVariable variable, TVarType type, void *value);

/***
 * VariablesGetType()
 *   funkce get_type (pro runtime) 
 *   vrati typ promene
 *   1. parametr - promena 
 */
TVarType VariablesGetType(PVariable variable);

/***
 * VariablesGetValue()
 *   funkce get_value (pro runtime)
 *   vrati obsah promene (nemente ji sami!) 
 *   1. parametr - promena 
 */
void *VariablesGetValue(PVariable variable);

/***
 * VariablesDestroy()
 *   zrusi tabulku promenych i s jejim obsahem (vsechny promene)
 */
void VariablesDestroy(PVariableTable table);

#ifndef NDEBUG
/***
 * ConstantPrint()
 *   vypise typ a obsah konstanty na stdout, ladici funkce
 *   parametr adresa konstanty  
 */
void ConstantPrint(PVariable constant);
#endif

#endif
