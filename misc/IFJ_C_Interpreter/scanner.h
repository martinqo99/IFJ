/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: scanner.h - modul lexikalniho analyzatoru
 * Autor: David Barina, xbarin02
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

#include "variables.h"

/***
 * TScannerData
 *   interni data lexikalniho analyzatoru 
 */ 
typedef struct _TScannerData
        TScannerData, *PScannerData;

/***
 * TScannerToken
 *   vlastni token a jeho typy, pozor na tInvalid a tEOF, line je od 0 !!!
 */
typedef enum _TToken
        {
          tInvalid,            // kvuli zpetne kompatibilite, bude smazano
          // pozn. (int *)pData ukazuje na EOF nebo znak, kt. zpusobil tInvalid:
          tInvalidString,      // znak s hodnotou <0x20 v retezci (pData)
          tInvalidEscape,      // neplatna escape sekv. v ret. (->\n") (pData)
          tInvalidChar,        // znak neni soucasti jazyka (pData)
          tInvalidExcl,        // po ! ocekavan = (pData)
          tInvalidColon,       // po : ocekavan : nebo = (pData)
          tInvalidExp,         // v real. cisle po e ocek. +-0..9 (pData)
          tInvalidSign,        // v real. cisle po e+- ocekavan 0..9 (pData)
          tInvalidInt,         // neni platny integer (NULL)
          tInvalidFloat,       // neni platne real. cislo (NULL)
          // platne tokeny:
          tLitInt,             // integer
          tLitFloat,           // float
          tLitString,          // string
          tIdentifier,         // ID
          tFuncSort,           // radici funkce Heapsort
          tKeyGoTo,            // klicove slovo go_to
          tKeyOn,              // on
          tKeyPrint,           // print
          tKeyRead,            // read
          tEOF,                // ! konec souboru
          tLabel,              // ::
          tParenthesisLeft,    // (
          tParenthesisRight,   // )
          tSemicolon,          // ; (oddelovac prikazu)
          tAssignment,         // := (prirazeni)
          tComma,              // , (oddelovac parametru funkce)
          tOpEqual,            // operator =
          tOpNotEqual,         // operator !=
          tOpLess,             // operator <
          tOpLessEqual,        // operator <=
          tOpGreater,          // operator >
          tOpGreaterEqual,     // operator >=
          tOpPlus,             // operator +
          tOpMinus,            // operator -
          tOpMultiply,         // operator *
          tOpDivide            // operator /
        } TToken;

struct _TScannerToken
       {
         TToken tType;
         void *pData;
         int line;
       };

typedef struct _TScannerToken
        TScannerToken, *PScannerToken;

/***
 * ScannerInit()
 *   inicializacni funkce lexikalniho analyzatoru pred prvnim volanim
 *   funkce ScannerGetNextToken()
 *   - parametr ukazatel na otevreny soubor 
 *   - vraci vytvorena interni data 
 */ 
TScannerData* ScannerInit(FILE *f);

/***
 * ScannerDestroy()
 *   funkce, ktera uvolni pamet alokovanou funkci ScannerInit 
 *    - parametr vytvorena interni data
 */ 
void ScannerDestroy(TScannerData *data);

/***
 * ScannerGetNextToken()
 *   funkce, ktera nacte a vrati dalsi token typu TScannerToken, ktery musi
 *   byt alkovany! (jeho hodnoty budou prepsany!)
 *   - 1. parametr adresa internich dat vracehych funkci ScannerInit()
 *   - 2. parametr adresa tokenu
 */ 
void ScannerGetNextToken(TScannerData *data, TScannerToken *token);

#endif
