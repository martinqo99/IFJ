/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   scanner.h
 *
 * Popis:
 *
 *
 * Datum:    20.11.2012
 *
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "errors.h"
#include "strings.h"
#include "mmu.h"

#define RESERVED_COUNT 9

extern FILE* gFileHandler;

typedef enum tkeyword{
    KW_IF, KW_ELSE, KW_END,                                             //Podminky
    KW_WHILE,                                                           //Cykly
    KW_FUNCTION, KW_RETURN,                                             //Funkce
    KW_FIND, KW_SORT, KW_INPUT, KW_NUMERIC, KW_PRINT, KW_TYPEOF, KW_LEN,//Vestavene funkce
    KW_NIL, KW_TRUE, KW_FALSE,

    LEX_L_SBRACKET, LEX_R_SBRACKET,
    LEX_NUMBER,LEX_STRING, LEX_ID, 
    LEX_L_BRACKET, LEX_R_BRACKET,
    LEX_ADDITION, LEX_SUBSTRACTION, LEX_MULTIPLICATION, LEX_DIVISION, LEX_POWER,
    LEX_EQUAL, LEX_UNEQUAL,
    LEX_LESSER, LEX_LESSER_EQUAL, LEX_GREATER, LEX_GREATER_EQUAL,LEX_EOL,
    LEX_ASSIGN, LEX_COMMA, LEX_COLON,
     LEX_EOF, //znaky konce radku a souboru

    LEX_UNKNOWN, //nepovoleny lexem
    LEX_RESERVED,
    LEX_ERROR,
    EXPRESSION
} tKeyword;

typedef enum tstate{
    S_START = 0,// 0 Start

    S_LESSER,
    S_GREATER,
    S_EQUAL,
    S_UNEQUAL,

    S_STAR,
    S_SUBSTRACTION,

    S_SLASH,
    S_COMMENT_ROW,
    S_COMMENT_BLOCK,
    S_COMMENT_END,

    S_STRING,
    S_STRING_ESCAPED,
    S_STRING_ASCII,

    S_ID,
    S_NUMBER,
    S_NUMBER_POINT,
    S_NUMER_DECIMAL,
    S_NUMBER_EXPONENT
} tState;

typedef struct ttoken{
  unsigned int row;
  unsigned int column;

  tString data;
} tToken;

extern tToken gToken;

int isReserved(tString);

//API pro praci s lexilanim analyzatorem
void initToken();
void resetToken();
void pushToken(int);
tKeyword getToken();
tKeyword getTokenAhead();

#endif
