#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "str.h"

#define LEX_ERROR -1
#define ERR_MALLOC -5

#define NONE 0
#define RESERVED_MAX 6
#define ASCII_MIN 32
#define ASCII_MAX 255
#define HUNDRED 100
#define TEN 10

/*FALSE = 0, TRUE = 1*/
enum {FALSE, TRUE};

/*vycet typu lexem*/
enum ELexem {

   KW_MAIN,  KW_FUNCTION, KW_LOCAL, KW_RETURN, // funkce
   KW_IF,    KW_THEN,     KW_ELSE,             // podminka
   KW_WHILE, KW_DO,	      KW_END, 			     // cyklus
   KW_READ,  KW_WRITE,                         // prikazy
   KW_TYPE,  KW_SUBSTR,	  KW_FIND,  KW_SORT,   // vestavene funkce
   KW_TRUE,  KW_FALSE,    KW_REPEAT,KW_UNTIL,
   KW_NIL,                                     // konstanty
   //KW_AND,   KW_OR,       KW_NOT,            // logika
   //18

   L_NUMBER,						// cislo
   L_STRING,						// retezec
   L_ID,								// identifikator

   L_LEFT_BRACKET,			// (
   L_RIGHT_BRACKET,			// )

   L_ADDITION,					// +
   L_SUBTRACTION,				// -
   L_MULTIPLICATION,		// *
   L_DIVISION,					// /
   L_POWER,							// ^
   L_CONCATENATION,			// ..

   L_SMALLER,						// <
   L_SMALLER_EQUAL,			// <=
   L_BIGGER,						// >
   L_BIGGER_EQUAL,			// >=
   L_EQUAL,							// ==
   L_UNEQUAL,						// ~=

   L_ASSIGN,						// =
   L_SEMICOLON,					// ;
   L_COMMA,							// ,

   //L_MODULO             //%
   //L_LENGTH             //#

   RESERVED_WORD,				// rezervovane slovo
   END_OF_FILE,					// EOF
};

/*vycet stavu*/
enum EState {
   S_DEFAULT,
   S_CONCATENATION,
   S_BIGGER,
   S_SMALLER,
   S_UNEQUAL,
   S_EQUAL,
   S_SUBTRACTION,
   S_COMMENT,
   S_COMMENT_ROW,
   S_COMMENT_BLOCK,
   S_COMMENT_BEGIN,
   S_COMMENT_END,
   S_STRING,
   S_ESCAPE,
   S_ESCAPE_NUMERAL,
   S_ESCAPE_DDD,
   S_ID,
   S_NUMBER,
   S_EXPONENT,
	 S_EXPONENT_BEGIN,
   S_EXPONENT_END,
   S_DECIMAL_POINT,
   S_DECIMAL_NUMBER
};

/*deklarace promennych*/
extern int countOfRows;

/*deklarace funkci*/
void setSourceFile(FILE *f);
int getNextToken(string *attr);

#endif
