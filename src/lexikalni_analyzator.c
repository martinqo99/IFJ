/**
 * Projekt: Implementace interpretu jazyka IFJ12
 * Soubor: lexikalni_analyzator.c
 * Autori: Frantisek Kolacek
 *         Petr Matyas
 *         Dalibor Skacel
 *         Michaela Muzikarova
 *         Veronika Necasova
**/

  // prilozeni hlavickovych souboru
#include "lexikalni_analyzator.h"

  // globalni promenne
tToken token;

  // klicova slova
const char *klicovaSlova[9] = {
"else\0", "end\0", "false\0", "function\0", 
"if\0", "nil\0", "return\0", "true\0", "while\0"
};

  // rezervovana slova
const char *rezervovanaSlova[9] = {
"as\0", "def\0", "directove\0", "export\0", "from\0", 
"import\0", "launch\0", "load\0", "macro\0"
};

  // funkce inicializuje token
void initToken(void) {
  token.stav = S_START;
  token.data = NULL;
}

/**
 * funkce vlozi znak na konec token.data
 @param c - vkladany znak
 @param i - kazatel na pocitadlo znaku
**/
void rozsirToken(int c, int *i) {
    //realokace pameti
  if (token.data = (char*) realloc(token.data, *i + 2)) {
    token.data[(*i) + 1] = '\0';    //posunuti koncoveho znaku
    token.data[(*i)] = c;           // vlozeni noveho znaku
    (*i)++;                         // inkrementace pocitadla znaku
    }
/**SEM JE TREBA DOPSAT CHYBOVY STAV, CHYBA PRI REALOKACI**/
}

  // fuknce naplni polozku token.stav
void naplToken (tStav stav) {
  token.stav = stav;
}

tToken getToken(void) {
  tStav stav = S_START;
  int i = 0;		// pocitadlo prectenych znaku
  int c;		// nacitany znak
  initToken();		// inicializace tokenu

    //spusteni konecneho automatu
  while (c = getc(soubor)) {
    switch (stav) {
      case S_START:		// 0 - pocatecni stav
      {
/**DOPSAT BOOL a NUMERIC - funkce isdigit??**/
        if (c == '=')       stav = S_PRIRAZENI;
        else if (c == '.')  stav = S_TECKA;
        else if (c == '(')  stav = S_LZ;
        else if (c == ')')  stav = S_PZ;
        else if (c == ';')  stav = S_STREDNIK;
        else if (c == ',')  stav = S_CARKA;
        else if (c == '!')  stav = S_VYKRICNIK;
        else if (c == '+')  stav = S_SOUCET;
        else if (c == '-')  stav = S_ROZDIL;
        else if (c == '*')  stav = S_SOUCIN;
        else if (c == '/')  stav = S_PODIL;
        else if (c == '**') stav = S_MOCNINA;
        else if (c == '<')  stav = S_MENSI;
        else if (c == '>')  stav = S_VETSI;
        else if (c == '<=') stav = S_MENSIROVNO;
        else if (c == '>=') stav = S_VETSIROVNO;
        else if (c == '!=') stav = S_NEROVNASE;
        else if (c == '==') stav = S_ROVNASE;
        else if (c == '"')  stav = S_STR;
        else if (c == ' ') {
          stav = SStart;
          break;
          }
        rozsirToken(c, &i);	//vlozeni znaku do token.data
	break;
      }
/**SEM JE POTREBA DOPSAT CHYBU PRO PRIPAD, ZE ZNAK NEODPOVIDA ANI JEDNOMU**/
      case S_SOUCET:
      case S_ROZDIL:
      case S_SOUCIN:
      case S_PODIL:
      {
	naplnToken(stav);
	stav = S_END;
	break;
      }

      case S_MENSI:
      {
	if (c == '=') {
	  stav = S_MENSIROVNO;
	  rozsirToken(c, &i);
	  }
	else {
	  naplnToken(stav);
	  stav = E_END;
	  }
	break;
      }

      case S_VETSI:
      {
	if (c == '=') {
	  stav = S_VETSIROVNO;
	  rozsirToken(c, &i);
	  }
	else {
	  naplnToken(stav);
	  stav = E_END;
          }
      }
      }        // konec switche
    } //konec while

}

