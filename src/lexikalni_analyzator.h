/**
 * Projekt: Implementace interpretu jazyka Falkon
 * Autori: Frantisek Kolacek
 *         Petr Matyas
 *         Dalibor Skacel
 *         Michaela Muzikarova
 *         Veronika Necasova
**/

/**
 * stavy automatu
 * vyctovy typ
**/
typedef enum {
  S_START;	//  0 -pocatecni stav
  S_KW;		//  1 - klicove slovo
  S_EOF;	//  2 - konec souboru
  S_ERR;	//  3 - chyba
  S_END;	//  4 - koncovy stav
  S_NUM;	//  5 - numeric
  S_STR;	//  6 - retezec
  S_BOOL;	//  7 - boolean
  S_PRIRAZENI;	//  8 - =
  S_TECKA;	//  9 - .
  S_LZ;		// 10 - (
  S_PZ;		// 11 - )
  S_STREDNIK; 	// 12 - ;
  S_CARKA;	// 13 - ,
  S_VYKRICNIK;	// 14 - !
  S_SOUCET;	// 15 - +
  S_ROZDIL;	// 16 - - 
  S_SOUCIN;	// 17 - *
  S_PODIL;	// 18 - /
  S_MOCNINA;	// 19 - **
  S_MENSI;	// 20 - <
  S_VETSI;       // 21 - >
  S_MENSIROVNO;	// 22 - <=
  S_VETSIROVNO;	// 23 - >=
  S_NEROVNASE;	// 24 - !=
  S_ROVNASE;	// 25 - ==
} tStav;


/**
 * struktura tToken
 @stav - stav konecneho automatu
         vyctovy typ tStav
 @data - hodnota lexemu
         char *, retezec
**/   
typedef struct {
  tStav stav;
  char *data;
} tToken;

// globalni promenna token
extern tToken token;
