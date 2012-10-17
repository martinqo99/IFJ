/*
 * @description   Knihovna pro práci s datovým typem string
 * @author        pøevzato z jednoduchy_interpret/str.h
                  Vendula Poncová - xponco00
 *                Patrik Hronsky - xhrons00
 * @projekt       IFJ11
 * @date
 */

#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#include <string.h>
#include <malloc.h>
#include <stdio.h>

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_EOF      1
#define STR_SUCCESS  0
#define STR_ERROR   -1
#define STR_EALLOC  -2

// Makro vrací true, pokud øetìzec ve string ukazuje na NULL
#define strIsNull(s) ((s)->str == NULL)

// struktura pro datový typ STRING:

typedef struct {
   char* str;		  // misto pro dany retezec ukonceny znakem '\0'
   int length;		  // skutecna delka retezce
   int allocSize;	// velikost alokovane pameti
} string;


// funkce pro práci se stringem:

inline int  strInitLen    (string *s, int len);

inline void strFree       (string *s);
inline void strClear      (string *s);
inline int  strCmpString  (string *s1, string *s2);

inline int  strAddChar    (string *s1, char c);
inline char *strCopyChar  (string *s);
inline string strCreateString  (string *str);

int  strInit            (string *s);
int  strCopyString      (string *s1, string *s2);

string strConcatenation (string *s1, string *s2);
string strCreateConstString (char *str);

int strReadNumber (FILE *f, double *dest);
int strReadNChar  (FILE *f, string *s, int n);
int strReadLine   (FILE *f, string *s);
int strReadAll    (FILE *f, string *s);

////////////////////////////////////////////////////////////////////////
/* inline funkce */

/*
 * Funkce vytvoøí prázdný string
 * o velikosti dané parametrem.
 * Pro len < 0 nealokuje nic,
 * pro len = 0 vznikne prazdny retezec,
 * jinak alokuje se rezetec dane delky.
 * @author  Vendula Poncová
 * @param   ukazatel na string
 * @param   delka noveho retezce
 * @return  chybový kód
 */
inline int strInitLen(string *s, int len) {

   // provede alokaci
   if (len >= 0) {
      if ((s->str = (char*) malloc(sizeof(char)*(len + 1))) == NULL)
         return STR_ERROR;

      s->allocSize = len + 1;
      s->str[0] = '\0';
   }
   // bez alokace
   else {
      s->allocSize = 0;
      s->str = NULL;
   }

   s->length = 0;
   return STR_SUCCESS;
}

/*
 * Funkce uvolní øetìzec z pamìti
 * a jeho hodnoty vynuluje.
 * @param   ukazatel na string
 */
inline void strFree(string *s) {
   free(s->str);
   s->length = 0;
   s->allocSize = 0;
}

/*
 * Funkce "vyma¾e" obsah øetìzce.
 * Velikost alokovaného prostoru se nezmìní.
 * @param   ukazatel na string
 */
inline void strClear(string *s) {
   s->str[0] = '\0';
   s->length = 0;
}

/*
 * Funkce porovná dva stringy a vrátí výsledek.
 * @param   ukazatel na string
 * @param   ukazatel na string
 * @param   integer, výsledek porovnání
 */
inline int strCmpString(string *s1, string *s2) {
   return strcmp(s1->str, s2->str);
}

/*
 * Funkce pøidá na konec øetìzce jeden znak.
 * Pokud pamì» nestaèí, provede realokaci.
 * @param   ukazatel na string
 * @param   znak
 */
inline int strAddChar(string *s1, char c) {

   if (s1->length + 1 >= s1->allocSize) {
      // pamet nestaci, je potreba provest realokaci
      s1->allocSize = s1->length + STR_LEN_INC;

      char *oldPtr = s1->str;
      if ((s1->str = (char*) realloc(s1->str, sizeof(char) * s1->allocSize)) == NULL) {
         free(oldPtr);
         return STR_ERROR;
      }
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}


/*
 * Funkce vytvoøí nový øetìzec a zkopíruje
 * do nìj øetìzec ze stringu.
 * @author  Patrik Hronský
 * @param   ukazatel na string
 * @return  øetìzec
 */
inline char *strCopyChar(string *s) {

   char *strNew = NULL;
   if ((strNew = (char *) malloc(sizeof(char)*(s->length + 1))) == NULL) {
      return NULL;
   }
   strcpy(strNew,s->str);
   return strNew;
}

/*
 * Funkce vytvoøí a vrátí nový string, jeho¾
 * øetìzcem bude kopie zadaného.
 * Je tøeba uvolnit voláním strFree() !
 * @author  Vendula Poncová
 * @param   øetìzec, který se zkopíruje
 * @return  nový string
 */
inline string strCreateString (string *sCopy) {

   string s;
   s.length = sCopy->length;
   s.allocSize = s.length + 1;
   s.str = strCopyChar(sCopy);

   return s;
}

#endif //STR_H_INCLUDED
