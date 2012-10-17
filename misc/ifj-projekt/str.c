/*
 * @description   Knihovna pro práci s datovým typem string
 * @author        pøevzato z jednoduchy_interpret/str.c
                  Vendula Poncová - xponco00
 *                Patrik Hronsky - xhrons00
 *                Tomas Trkal - xtrkal00
 * @projekt       IFJ11
 * @date
 */

#include "str.h"
#include "scanner.h"

/*
 * Funkce vytvoøí prázdný string
 * o velikosti STR_LEN_INC
 * @param   ukazatel na string
 * @return  chybový kód
 */
int strInit(string *s) {

   if ((s->str = (char*) malloc(sizeof(char)*STR_LEN_INC)) == NULL)
      return STR_ERROR;

   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;

   return STR_SUCCESS;
}

/*
 * Funkce pøekopíruje do stringu s1 obsah stringu s2
 * Pokud pamì» nestaèí, provede realokaci.
 * @param   ukazatel na string, KAM kopírujeme
 * @param   ukazatel na string, ODKUD kopírujeme
 * @param   chybový kód
 */
int strCopyString(string *s1, string *s2) {

   int newLength = s2->length;
   if (newLength >= s1->allocSize) {
      // pamet nestaci, je potreba provest realokaci
      s1->allocSize = newLength + 1;
      char *oldPtr = s1->str;
      if ((s1->str = (char*) realloc(s1->str, sizeof(char)*s1->allocSize)) == NULL) {
         free(oldPtr);
         return STR_ERROR;
      }
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}


/*
 * Funkce provede konkatenaci dvou stringù
 * a vrátí nový string.
 * @author  Vendula Poncová
 * @param   ukazatel na string
 * @param   ukazatel na string
 * @param   výsledný string
 */
string strConcatenation (string *s1, string *s2) {

   string s;
   s.length = s1->length + s2->length;
   s.allocSize = s.length + 1;

   if ( (s.str = (char*)malloc(sizeof(char)*s.allocSize)) != NULL ) {

      strcpy(s.str, s1->str);
      strcpy(&s.str[s1->length], s2->str);

   } else s.str = NULL;

   return s;
}

/*
 * Funkce vytvoøí a vrátí nový string, jeho¾
 * øetìzec bude ukazovat na zadaný.
 * Øetìzec nelze mìnit. Nemusí se uvolòovat.
 * @author  Vendula Poncová
 * @param   øetìzec, který se zkopíruje
 * @return  nový string
 */
string strCreateConstString (char *str) {
   string s;
   s.length = strlen(str);
   s.allocSize = s.length + 1;
   s.str = str;

   return s;
}

/*
 * Funkce naète do stringu n znakù.
 * @author  Vendula Poncová
 * @param   vstupní soubor
 * @param   string
 * @param   poèet naètených znakù
 * @return  chybový kód
 */
int strReadNChar(FILE *f, string *s, int n) {

   if (n < 0)
     return STR_ERROR;

   // inicializujeme a alokujeme string
   if (strInitLen(s, n) != STR_SUCCESS)
      return STR_EALLOC;

   // naèteme a zkopírujeme znaky:
   int i, c = '\0';

   for (i = 0; (i < n) && ((c = fgetc(f)) != EOF); i++) {
      s->str[i] = (char)c;
   }
   s->str[i] = '\0';
   s->length = i;

   // naèten pouze EOF
   if (c == EOF && s->length == 0) {
      strFree(s);
      return STR_EOF;
   }

   return STR_SUCCESS;
}

/*
 * Funkce naèítá znaky do konce øádku
 * a ukládá je do stringu.
 * @author  Vendula Poncová
 * @param   vstupní soubor
 * @param   string
 * @return  chybový kód
 */
int strReadLine(FILE *f, string *s) {

   int err = STR_SUCCESS;

   // inicializujeme a alokujeme string
   if (strInit(s) != STR_SUCCESS)
      return STR_EALLOC;

   // naèteme a zkopírujeme znaky:
   int c;
   while ( ((c = fgetc(f)) != EOF) && (c !='\n') && err == STR_SUCCESS ) {
      err = strAddChar(s, c);
   }

   // pokud do¹lo k chybì, uvolníme data
   if (err == STR_ERROR) {
      strFree(s);
      return STR_EALLOC;
   }
   // bylo naèteno jen EOF
   else if (c == EOF && s->length == 0) {
      strFree(s);
      err = STR_EOF;
   }

   return err;
}

/*
 * Funkce naèítá znaky do konce souboru (EOF)
 *  a vrátí je ve vytvoøeném øetìzci.
 * @author  Vendula Poncová
 * @param   vstupní soubor
 * @param   string
 * @return  chybový kód
 */
int strReadAll(FILE *f, string *s) {
   int err = STR_SUCCESS;

   // inicializujeme a alokujeme string
   if (strInit(s) != STR_SUCCESS)
      return STR_EALLOC;


   // naèteme a zkopírujeme znaky:
   int c;
   while(((c = fgetc(f)) != EOF)) {
      err = strAddChar(s, c);
   }
   // pokud do¹lo k chybì, uvolníme data
   if (err == STR_ERROR) {
      strFree(s);
      return STR_EALLOC;
   }
   
   return err;
}

/*
 * Funkce nacte ze vstupu cislo a vrati ho
 * @author Tomas Trkal
 * @param  vstupni soubor
 * @param  kam se ulozi vysledne cislo
 * @return chybovy kod
 */
int strReadNumber (FILE *f, double *dest) {
	int err = STR_SUCCESS;

  // inicializace retezce
	string s;
	err = strInit(&s);

	if (err != STR_SUCCESS)
     return STR_EALLOC;

	int state = S_DEFAULT;
  int isNumber = FALSE;
  int c;

  // nacitani cisla
	while (err == STR_SUCCESS && isNumber == FALSE) {
		c = getc(f);

		/*ridici swich*/
    switch (state) {
        
			/*S_DEFAULT*/
			case S_DEFAULT:
         if (isspace(c))
            continue;

         if (isdigit(c)) {
  			    if (strAddChar(&s,c))
						   err = STR_EALLOC;
				       state = S_NUMBER;
				 }
				 else {
				    ungetc(c,f);
					  err = STR_ERROR;
				 }
			break;

			/*S_NUMBER*/
			case S_NUMBER:
			   if (isdigit(c)) {
			      if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
			   } 
				 else if (c == '.') {
			      if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
			      state = S_DECIMAL_POINT;
			   } 					
				 else if ((c == 'e') || (c == 'E')) {
			      if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
			      state = S_EXPONENT;
  		   } 
				 else {
			     ungetc(c,f);
			     isNumber = TRUE;
			   }
			break;

			/*S_DECIMAL_POINT*/
			case S_DECIMAL_POINT:
			   if (isdigit(c)) {
				    if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
				    state = S_DECIMAL_NUMBER;
			 	 } 
 			   else {
					  ungetc(c,f);
					  err = STR_ERROR;
				 }
			break;

			/*S_DECIMAL_NUMBER*/
			case S_DECIMAL_NUMBER:
         if (isdigit(c)) {
            if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
         }
         else if ((c == 'e') || (c == 'E')) {
	          if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
	          state = S_EXPONENT;
         } 
         else {
            ungetc(c,f);
            isNumber = TRUE;
	       }
       break;

       /*S_EXPONENT*/
       case S_EXPONENT:
          if (isdigit(c)) {
				    if (strAddChar(&s,c))
						   err = STR_EALLOC;
					  state = S_EXPONENT_END;
				 }	
				 else if ((c == '+') || (c == '-')) {
            if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
            state = S_EXPONENT_BEGIN;
         } 
				 else {
					  ungetc(c,f);
				 	  err = STR_ERROR;
				 }
			break;

       /*S_EXPONENT_BEGIN*/
			case S_EXPONENT_BEGIN:
			   if (isdigit(c)) {
				    if (strAddChar(&s,c))
						   err = STR_EALLOC;
					  state = S_EXPONENT_END;
				 }
				 else {
				    ungetc(c,f);
					  err = STR_ERROR;
				 }
			break;

			/*S_EXPONENT_END*/
			case S_EXPONENT_END:
			   if (isdigit(c)) {
				    if (strAddChar(&s,c)) 
						   err = STR_EALLOC;
				 } 
			   else {
				    ungetc(c,f);
				    isNumber = TRUE;
				 }
			break;			

      }	/*konec switche*/
	  } /*konec while*/

   // cislo uspesne nactene
   // konvertuj
	 if (isNumber) {
      *dest = atof(s.str);
   }

   strFree(&s);
   return err;
}

/* konec souboru str.c */
