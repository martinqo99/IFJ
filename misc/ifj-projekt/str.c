/*
 * @description   Knihovna pro pr�ci s datov�m typem string
 * @author        p�evzato z jednoduchy_interpret/str.c
                  Vendula Poncov� - xponco00
 *                Patrik Hronsky - xhrons00
 *                Tomas Trkal - xtrkal00
 * @projekt       IFJ11
 * @date
 */

#include "str.h"
#include "scanner.h"

/*
 * Funkce vytvo�� pr�zdn� string
 * o velikosti STR_LEN_INC
 * @param   ukazatel na string
 * @return  chybov� k�d
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
 * Funkce p�ekop�ruje do stringu s1 obsah stringu s2
 * Pokud pam� nesta��, provede realokaci.
 * @param   ukazatel na string, KAM kop�rujeme
 * @param   ukazatel na string, ODKUD kop�rujeme
 * @param   chybov� k�d
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
 * Funkce provede konkatenaci dvou string�
 * a vr�t� nov� string.
 * @author  Vendula Poncov�
 * @param   ukazatel na string
 * @param   ukazatel na string
 * @param   v�sledn� string
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
 * Funkce vytvo�� a vr�t� nov� string, jeho�
 * �et�zec bude ukazovat na zadan�.
 * �et�zec nelze m�nit. Nemus� se uvol�ovat.
 * @author  Vendula Poncov�
 * @param   �et�zec, kter� se zkop�ruje
 * @return  nov� string
 */
string strCreateConstString (char *str) {
   string s;
   s.length = strlen(str);
   s.allocSize = s.length + 1;
   s.str = str;

   return s;
}

/*
 * Funkce na�te do stringu n znak�.
 * @author  Vendula Poncov�
 * @param   vstupn� soubor
 * @param   string
 * @param   po�et na�ten�ch znak�
 * @return  chybov� k�d
 */
int strReadNChar(FILE *f, string *s, int n) {

   if (n < 0)
     return STR_ERROR;

   // inicializujeme a alokujeme string
   if (strInitLen(s, n) != STR_SUCCESS)
      return STR_EALLOC;

   // na�teme a zkop�rujeme znaky:
   int i, c = '\0';

   for (i = 0; (i < n) && ((c = fgetc(f)) != EOF); i++) {
      s->str[i] = (char)c;
   }
   s->str[i] = '\0';
   s->length = i;

   // na�ten pouze EOF
   if (c == EOF && s->length == 0) {
      strFree(s);
      return STR_EOF;
   }

   return STR_SUCCESS;
}

/*
 * Funkce na��t� znaky do konce ��dku
 * a ukl�d� je do stringu.
 * @author  Vendula Poncov�
 * @param   vstupn� soubor
 * @param   string
 * @return  chybov� k�d
 */
int strReadLine(FILE *f, string *s) {

   int err = STR_SUCCESS;

   // inicializujeme a alokujeme string
   if (strInit(s) != STR_SUCCESS)
      return STR_EALLOC;

   // na�teme a zkop�rujeme znaky:
   int c;
   while ( ((c = fgetc(f)) != EOF) && (c !='\n') && err == STR_SUCCESS ) {
      err = strAddChar(s, c);
   }

   // pokud do�lo k chyb�, uvoln�me data
   if (err == STR_ERROR) {
      strFree(s);
      return STR_EALLOC;
   }
   // bylo na�teno jen EOF
   else if (c == EOF && s->length == 0) {
      strFree(s);
      err = STR_EOF;
   }

   return err;
}

/*
 * Funkce na��t� znaky do konce souboru (EOF)
 *  a vr�t� je ve vytvo�en�m �et�zci.
 * @author  Vendula Poncov�
 * @param   vstupn� soubor
 * @param   string
 * @return  chybov� k�d
 */
int strReadAll(FILE *f, string *s) {
   int err = STR_SUCCESS;

   // inicializujeme a alokujeme string
   if (strInit(s) != STR_SUCCESS)
      return STR_EALLOC;


   // na�teme a zkop�rujeme znaky:
   int c;
   while(((c = fgetc(f)) != EOF)) {
      err = strAddChar(s, c);
   }
   // pokud do�lo k chyb�, uvoln�me data
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
