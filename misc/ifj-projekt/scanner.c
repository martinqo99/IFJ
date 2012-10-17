//=================================================================================================>
//-------------------------------------LEXIKALNI ANALYZATOR---------------------------------------->
//=================================================================================================>
/* @author: Tomas Trkal, xtrkal00@stud.fit.vutbr.cz
 * @correction: Vendula Poncova
 * @date: 	11.11.2011
 */

#include "scanner.h"

/*nastavi pocitadlo radku na jedna*/
int countOfRows = 1;

/*tabulka s klicovymi a rezervovanymi slovy*/
const char *reservedWords[] = {"break", "elseif", "for", "in", "or", "not", "and"};
const char *keyWords[] = {
   //[KW_OR] = "or",              [KW_AND] = "and",       [KW_NOT] = "not",
   [KW_END] = "end",					[KW_LOCAL] = "local",      [KW_WRITE] = "write",
   [KW_RETURN] = "return",			[KW_IF] = "if", 				[KW_THEN] ="then",
   [KW_FUNCTION] = "function",	[KW_READ] = "read",			[KW_DO] = "do",
   [KW_ELSE] = "else",    			[KW_WHILE] = "while",		[KW_NIL] = "nil",
   [KW_FALSE] = "false", 			[KW_TRUE] = "true", 		   [KW_TYPE] = "type",
   [KW_SUBSTR] = "substr",			[KW_FIND] = "find",			[KW_SORT] = "sort",
   [KW_MAIN] = "main",           [KW_REPEAT] = "repeat",    [KW_UNTIL] = "until",
};

/*promenna pro ulozeni vstupniho souboru*/
FILE *source;
void setSourceFile(FILE *f) {
   source = f;
}

//=================================================================================================>
//-----------------------------------int isKeyReserved(char *word);-------------------------------->
//=================================================================================================>
/*zjisti jestli je retezec ID, key, reserved*/
int isKeyOrReserved(char *word) {
   int i; //iteracni promenna

   /*porovna se vsemi rezervovanymi slovy*/
   for (i = 0; i <= RESERVED_MAX; i++) {
      if (strcmp(reservedWords[i], word) == 0)
         return RESERVED_WORD;
   }

   /*porovna se vsemi klicovymi slovy*/
   for (i = KW_MAIN; i <= KW_NIL; i++) {
      if (strcmp(keyWords[i], word) == 0)
         return i;
   }
   return L_ID;
}

//================================================================================================>
//-----------------------------------int getNextToken(string *attr);------------------------------>
//================================================================================================>
/*hlavni funkce lexikalniho analyzatoru*/
int getNextToken(string *attr) {

   /*nasteveni vychoziho stavu*/
   strClear(attr);
   int state = S_DEFAULT;	//defaultni stav
   int ascii = NONE;
   int c;

   /*konecny automat*/
   while (TRUE) {
      c = getc(source);
      switch (state) {

         /*vychozi stav automatu*/
      case S_DEFAULT:
         if (isspace(c)) { //ignorace bilych mist
            if (c == '\n') countOfRows++;
            state = S_DEFAULT;
         } else if (c == EOF) return END_OF_FILE;
         else if (c == '(') return L_LEFT_BRACKET;
         else if (c == ')') return L_RIGHT_BRACKET;
         else if (c == ';') return L_SEMICOLON;
         else if (c == '^') return L_POWER;
         else if (c == '*') return L_MULTIPLICATION;
         else if (c == '/') return	L_DIVISION;
         else if (c == '+') return L_ADDITION;
         else if (c == ',') return L_COMMA;
				 //else if (c == '%') return L_MODULO;
				 //else if (c == '#') return L_LENGTH;
         else if (c == '.') state = S_CONCATENATION; 	// ..
         else if (c == '<') state = S_SMALLER;					// < || <=
         else if (c == '>') state = S_BIGGER;					// > || >=
         else if (c == '~') state = S_UNEQUAL;					// ~=
         else if (c == '=') state = S_EQUAL;						// = || ==
         else if (c == '-') state = S_SUBTRACTION;			// - || -- || --[[
         else if (c == '"') state = S_STRING; 					//retezec
         else if (isalpha(c)) { 												//ID || klicove slovo
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_ID;
         } else if (isdigit(c)) { //cisla
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_NUMBER;
         } else return LEX_ERROR; //jiny znak
         break;

      /*S_CONCATENATION*/
      case S_CONCATENATION:
         if (c == '.') return L_CONCATENATION;
         else return LEX_ERROR;
         break;

      /*S_SMALLER*/
      case S_SMALLER:
         if (c == '=') return L_SMALLER_EQUAL;
         else {
            ungetc(c,source);
            return L_SMALLER;
         }
         break;

      /*S_BIGGER*/
      case S_BIGGER:
         if (c == '=') return L_BIGGER_EQUAL;
         else {
            ungetc(c,source);
            return L_BIGGER;
         }
         break;

      /*S_UNEQUAL*/
      case S_UNEQUAL:
         if (c == '=') return L_UNEQUAL;
         else return LEX_ERROR;
         break;

      /*S_EQUAL*/
      case S_EQUAL:
         if (c == '=') return L_EQUAL;
         else {
            ungetc(c,source);
            return L_ASSIGN;
         }
         break;

      /*S_SUBTRACTION*/
      case S_SUBTRACTION:
         if (c == '-') state = S_COMMENT;
         else {
            ungetc(c,source);
            return L_SUBTRACTION;
         }
         break;

     /*stavy pro retezce*/
      /*S_STRING*/
      case S_STRING:
         if (c == '\n') countOfRows++;
         if (c == EOF) return LEX_ERROR;
         else if (c == '"') return L_STRING;
         else if (c == '\\') state = S_ESCAPE;
         else {
            if (strAddChar(attr,c)) return ERR_MALLOC;
         }
         break;

      /*S_ESCAPE*/
      case S_ESCAPE:
         if (c == 'n') {
            if (strAddChar(attr,'\n')) return ERR_MALLOC;
            state = S_STRING;
         } else if (c == 't') {
            if (strAddChar(attr,'\t')) return ERR_MALLOC;
            state = S_STRING;
         } else if (c == '\\') {
            if (strAddChar(attr,'\\')) return ERR_MALLOC;
            state = S_STRING;
         } else if (c == '"') {
            if (strAddChar(attr,'\"')) return ERR_MALLOC;
            state = S_STRING;
         } else if (isdigit(c)) {
            ascii = (c - '0') * HUNDRED;
            state = S_ESCAPE_NUMERAL;
         } else return LEX_ERROR;
         break;

      /*S_ESCAPE_NUMERAL*/
      case S_ESCAPE_NUMERAL:
         if (isdigit(c)) {
            ascii += (c - '0') * TEN;
            state = S_ESCAPE_DDD;
         } else return LEX_ERROR;
         break;

      /*S_ESCAPE_DDD*/
      case S_ESCAPE_DDD:
         if (isdigit(c)) {
            ascii += c - '0';
            if ((ascii >= ASCII_MIN) && (ascii <= ASCII_MAX)) {
               if (strAddChar(attr,ascii)) return ERR_MALLOC;
            }
            state = S_STRING;
         } else return LEX_ERROR;
         break;

     /*stavy pro cisla*/
      /*S_NUMBER*/
      case S_NUMBER:
         if (isdigit(c)) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
         } else if (c == '.') {
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_DECIMAL_POINT;
         } else if ((c == 'e') || (c == 'E')) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_EXPONENT;
         } else {
            ungetc(c,source);
            return L_NUMBER;
         }
         break;

      /*S_DECIMAL_POINT*/
      case S_DECIMAL_POINT:
         if (isdigit(c)) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_DECIMAL_NUMBER;
         } else return LEX_ERROR;
         break;

      /*S_DECIMAL_NUMBER*/
      case S_DECIMAL_NUMBER:
         if (isdigit(c)) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
         } else if ((c == 'e') || (c == 'E')) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_EXPONENT;
         } else {
            ungetc(c,source);
            return L_NUMBER;
         }
         break;

      /*S_EXPONENT*/
      case S_EXPONENT:
         if (isdigit(c)) {
				   if (strAddChar(attr,c)) return ERR_MALLOC;
					 state = S_EXPONENT_END;
				 }
				 else if ((c == '+') || (c == '-')) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
            state = S_EXPONENT_BEGIN;
         } else return LEX_ERROR;
         break;

			/*S_EXPONENT_BEGIN*/
			case S_EXPONENT_BEGIN:
				if (isdigit(c)) {
				  if (strAddChar(attr,c)) return ERR_MALLOC;
					state = S_EXPONENT_END;
				}				 
				else return LEX_ERROR;
			break;

      /*S_EXPONENT_END*/
      case S_EXPONENT_END:
         if (isdigit(c)) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
         } else {
            ungetc(c,source);
            return L_NUMBER;
         }
         break;

     /*stavy pro identifikatory*/
      /*S_ID*/
      case S_ID:
         if (isdigit(c) || isalpha(c) || (c == '_')) {
            if (strAddChar(attr,c)) return ERR_MALLOC;
         } else {
            ungetc(c,source);
            return isKeyOrReserved(attr->str);
         }
         break;

     /*stavy pro komentare*/
      /*S_COMMENT*/
      case S_COMMENT:
         if (c == EOF) return END_OF_FILE;
         else if (c == '[') state = S_COMMENT_BLOCK;
         else if (c == '\n') {
            countOfRows++;
            state = S_DEFAULT;
         } else state = S_COMMENT_ROW;
         break;

      /*S_COMMENT_BLOCK*/
      case S_COMMENT_BLOCK:
         if (c == EOF) return END_OF_FILE;
         else if (c == '[') state = S_COMMENT_BEGIN;
         else if (c == '\n') {
            countOfRows++;
            state = S_DEFAULT;
         } else state = S_COMMENT_ROW;
         break;

      /*S_COMMENT_BEGIN*/
      case S_COMMENT_BEGIN:
         if (c == '\n') countOfRows++;
         if (c == EOF) return LEX_ERROR;
         if (c == ']') state = S_COMMENT_END;
         break;

      /*S_COMMENT_END*/
      case S_COMMENT_END:
         if (c == '\n') countOfRows++;
         if (c == EOF) return LEX_ERROR;
         else if (c == ']') state = S_DEFAULT;
         else state = S_COMMENT_BEGIN;
         break;

      /*S_COMMENT_ROW*/
      case S_COMMENT_ROW:
         if (c == EOF) return END_OF_FILE;
         if (c == '\n') {
            countOfRows++;
            state = S_DEFAULT;
         }
         break;

      } /*konec switch*/
   } /*konec while*/
} /*konec getNextToken*/
