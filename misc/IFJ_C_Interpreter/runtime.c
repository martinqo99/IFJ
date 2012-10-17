/**
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xhefka00, xhradi08, xfilak01
 *
 * Soubor: runtime.c - modul runtimove interpretace
 * Autor: Jakub Filák, xfilak01
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "ifj05.h"
#include "scanner.h"
#include "symbols.h"
#include "variables.h"
#include "tape.h"
#include "runtime.h"




/**
 * Na zasobniku je ulozen odkaz na data a typ dat
 **/
typedef struct tOnStack {
  TVarType VarType;
  void* VarValue;
} *POnStack;

/**
 * Zasobnik si udrzeje pocet volnych mist a vrchol zasobniku a ukazatel na prvni structuru
 **/
typedef struct {
  int Top;		// Index vrcholu zasobnuiku
  int FreeSpace;	// Aktualni volne misto v zasobniku
  POnStack OnStack;	// Pole zasobniku
} TStack;

/**
 * Radici funkce 
 * prvni parametr je pole retezcu
 * druhy je pocet retezcu v poli
 * Funkce vraci serazeny retezec.
 **/
char* heapsort (char** array, unsigned long  string_sum);

/* Sito na vytvoreni heapu pro heap sort */
void sift (char** arr, unsigned long n, unsigned long i);

/**
 * Inicializace zasobniku
 * parametr funkce je ukazatel na strukturu zasobniku
 * funkce vytvori pole potrebne pro zasobnik, do vrcholu (Top) priradi hodnotu -1 coz
 * znamena ze je zasobnik prazdny, volne misto (FreeSpace) bude naplneno hodnotou 
 * naalokvane velikosti pole a celkova velikost pole (Allocated) bude stejna jako volne misto 
 **/
void StackInit (TStack* Stack);

/**
 * Ulozeni na vrchol zasobniku
 * Prvni parametr je ukazatel na strukturu zasobniku, druhy parametr je typ dat
 * promene ktera se bude ukladat na zasobnik, treti parametr je ukazatel na data
 * na zasbnik se uklada pouze ukazatel, data byla do pameti vlozena jiz pred volanim funkce
 **/
void StackPush (TStack* Stack, TVarType VarType, void* VarValue);

/** 
 * Odebrani vrcholu zasobniku
 * Prvni parametr je ukazatel na strukturu zasobniku, druhy parametr ukazatel na  
 * misto v pameti kam se ulozi typ dat, ktera jsou na zasobniku,
 * treti parametr je ukazatel na ukaztel na misto v pameti kde jsou ulozena data
 **/
void StackPop (TStack* Stack, TVarType* VarType, void** VarValue);

/**
 * Pridani mista na zasobniku
 * Kdyz dojde misto na zasobniku se vola funkce AddStackSpace() ktera misto prida,
 * parametr je ukazatel na strukturu zasobniku, prida se dvakrat tolik mista kolik je
 * momentalne vyuzito
 **/
void AddStackSpace (TStack* Stack);

/**
 * Uvolneni pameti zasobniku
 * Parametrem funkce je ukazatel na zasobnik. Uvolni veskerou pamet kterou zasobnik
 * vyuziva
 **/
void StackDestroy (TStack* Stack);

/**
 * Kopirovani retezcu 
 * Prvni parametrem je ukazatel na retezec kltery ma byt kopirovan
 * Druhy parametr pocet znaku pro alokaci pameti, delka kopirovaneho retezce.
 **/
inline char* CopyString ( const void* pStr, unsigned StrLen );

/**
 * Kopirovani celych cisel 
 * Parametrem je ukazatel na cele cislo
 **/
inline int* CopyInt ( const void* pInt );

/**
 * Kopirovani desetinych cisel
 * Parametrem je ukazatel na desetine cislo
 **/
inline double* CopyFloat ( const void* pFloat );

/* Alokace */
inline void* RuntimeMalloc ( unsigned long SIZE );

/**
 * RuntimeProcess
 * Hlavni funkce, provadi semantickopu analyzu a interpretaci vstupniho progrmau.
 * Instrukce ktere spracovava jsou mu predany v prvnim parametru jako pole.
 * V druhem parametru se funkci predava jmeno spracovavaneho souboru.
 **/
int RuntimeProcess (PInstTape InsTape,const char* FileName);

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
//                                            HEAPSORT                                     //
/////////////////////////////////////////////////////////////////////////////////////////////
char* heapsort ( char** array, unsigned long  string_sum )
{
  char* tmp = NULL;
  setlocale(LC_ALL, "cs_CZ");  // Nastaveni ceske lokalizace

  for (long i = (string_sum-1)/2; i >= 0; i--)
    sift (array, string_sum-1, i);

  for (unsigned long i = string_sum - 1; i > 0; i--) {
    tmp = array[0];
    array[0] = array[i];
    array[i] = tmp;
    sift (array, i-1, 0);
  }
  tmp = NULL;

  /* Miosto v pameti pro prvni ze serazenych retezcu */
  if ( ( tmp = (char*) malloc ((1+(sizeof(char)*strlen(array[0]))))) == NULL ) {
    fprintf (stderr, "error: not enought memory\n");
    exit(1);
  }
  /* Zkopirovani prvniho retezce a jeho nasledne uvlnoneni z pameti */
  strcpy( tmp, array[0] );
  free( array[0] );

  /* Prochazi serazene pole a spoje retezce dohromady, provadi konkatenaci retezcu v poli */
  for (unsigned long i = 1; i < string_sum; i++) {

    if ( ( tmp = realloc(tmp,(1+(sizeof(char)*(strlen(array[i])+strlen(tmp)))))) == NULL ) {
      fprintf (stderr, "error: not enought memory\n");
      exit(1);
    }

    strcat( tmp, array[i]); // konkatenace serazenych retezcu
    free(array[i]);
  }

  return tmp; // vraci retezec serazenych konkatenovanych retezcu
}

void sift (char** arr, unsigned long n, unsigned long i)
{
  char *tmp = arr[i];
  unsigned long j = 2*i+1; // kvuli indexaci od nuly 2*0 = 0 nikam by se neposunul

  while (j <= n ) {
    if (( j < n ) && ( strcoll(arr[j+1], arr[j]) > 0)) {
      j++;
    }

    if ( strcoll(arr[i], arr[j]) >= 0 )
      break;

    tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    i = j;
    j = 2*i+1;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                    STACK                                                //
/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Inicializace zasobniku
 **/ 
void StackInit (TStack *Stack)
{
  int NewSpace = 32;

  if ( (Stack->OnStack = (POnStack) malloc ( NewSpace*sizeof( struct tOnStack ))) == NULL) {
    fprintf (stderr, "error: not enought memory\n");
    exit (1);
  }

  Stack->FreeSpace = NewSpace;
  Stack->Top = -1; 
}

/**
 * Push, pridani na vrchol zasobniku,
 * kdyz neni dost mista vola se AddStackSpace()
 * ktera misto prida, uchovava odkazy
 **/ 
void StackPush (TStack *Stack, TVarType VarType, void* VarValue)
{
 if (Stack->FreeSpace == 0)
   AddStackSpace (Stack);

 Stack->Top++;
 Stack->FreeSpace--;

#ifndef NDEBUG
  if ( Stack == NULL ){
    fprintf(stderr,"Runtime internal stack error, StackPush\n");
    exit (1);
  }
#endif

   Stack->OnStack[Stack->Top].VarType = VarType;
   Stack->OnStack[Stack->Top].VarValue = VarValue;
}


/**
 * Pokus o POP, precteni a odstraneni z vrcholu zasobniku
 **/
void StackPop (TStack* Stack, TVarType* VarType, void** VarValue)
{

#ifndef NDEBUG
  if ( Stack == NULL ) {
    fprintf(stderr,"Runtime internal stack error, StackPop\n");
    exit (1);  
  }

  if ( Stack->Top == -1 ) {
    fprintf(stderr,"Runtime internal stack error, StackPop\n");
    exit (1);
  }
#endif

  *VarType =  Stack->OnStack[Stack->Top].VarType;

#ifndef NDEBUG
    if (((*VarValue = Stack->OnStack[Stack->Top].VarValue) == NULL) && (*VarType != vtUninit)){
      fprintf (stderr, "Internal error, StackPop\n");
      exit ( 1 );
    }
#else
    *VarValue = Stack->OnStack[Stack->Top].VarValue;
#endif

    Stack->Top--;
    Stack->FreeSpace++;
}

/**
 * pridava pameti
 **/ 
void AddStackSpace (TStack *Stack)
{
#ifndef NDEBUG
  if ( Stack == NULL ){
    fprintf(stderr,"Runtime internal stack error, AddStackSpace\n");
    exit (1);
  }
#endif

  int NewSpace = (Stack->Top+1)*2;
  Stack->FreeSpace = Stack->Top+1;

  if ((Stack->OnStack = (POnStack) realloc ( (Stack->OnStack), NewSpace*sizeof(struct tOnStack))) == NULL ) {
    fprintf (stderr, "error: not enought memory\n");
    exit(1);
  }
}

/**
 * Pokud nebyl zasobnik vybran, udela tolik pop dokud neni prazdny a potom 
 * uvilni zbytek zasobniku
 **/ 
void StackDestroy (TStack* Stack){
  void* tmp = NULL;
  TVarType VarType;

#ifndef NDEBUG
  if ( Stack == NULL ){
    fprintf(stderr,"Runtime internal stack error, StackDestroy\n");
    exit (1);
  }
#endif

  while ( Stack->Top >=  0 ){ // cyklus by se nemel nikdo provadet, znamenalo by to chybu 
    StackPop(Stack, &VarType, &tmp); // v syntakcticke analyze
    free(tmp);
  }

  free(Stack->OnStack);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                  INTERN FUNCTION                                        //
/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Kopiruje retezec a vraci na nej ukazatel
 */
inline char* CopyString ( const void* pStr, unsigned StrLen )
{
  char* string =(char*) malloc(sizeof(char)*(StrLen + 1));

  if ( string == NULL ) {
    fprintf (stderr, "error: not enought memory\n");
    exit(1);
  }

  strcpy(string,(char*) pStr );
  return string;
}

/**
 * Kopiruje integer a vraci na nej ukazatel
 */
inline int* CopyInt ( const void* pInt )
{
  int* i = (int *) malloc ( sizeof ( int ) );

  if ( i == NULL ) {
    fprintf (stderr, "error: not enought memory\n");
    exit(1);
  }

  *i = *(int*)pInt;
  return i;
}

/**
 * Kopiruje double a vraci na nej ukazatel
 */
inline double* CopyFloat ( const void* pFloat )
{
  double* d = (double *) malloc ( sizeof ( double ) );

  if ( d  == NULL ) {
    fprintf (stderr, "error: not enought memory\n");
    exit(1);
  }

  *d = *(double*) pFloat;
  return d;
}

/**
 * Alokuje pamet pro dany typ BUDU JESTE MENIT
 */
inline void* RuntimeMalloc ( unsigned long SIZE )
{
  void* ptr = malloc (SIZE);

  if ( ptr == NULL ){
    fprintf (stderr, "error: not enought memory\n");
    exit ( 1 );
  }

  return ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                    RUNTIME PROCES                                       //
/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Hlavni proces, vykonova interpretaci.
 **/
int RuntimeProcess (PInstTape InsTape,const char* FileName)
{
  unsigned TapeIndex = 0;
  int error = 0;

  char Temp[32]; // snad dostecne mnozstvi mista pro double a int v retezci

  PInstBlock ActInst; // Act jako aktualni instrukce

  TVarType ActType;
  TVarType SecondType;
  void* ActValue = NULL;    // Act jako aktualni data
  void* SecondValue = NULL; // a to je ta druha u binarnich operatoru

#ifndef NDEBUG
  if ( InsTape == NULL || FileName == NULL ) {
    fprintf (stderr, "Internal error RuntimeProces\n");
    exit ( 1 );
  }
#endif
  /* Zasobnik a jeho inicializace */
  TStack Stack;
  StackInit(&Stack);

  /* Pro funkci read, potrebuje to lexikalni anal. */
  TScannerData* Scanner = ScannerInit(stdin);
  TScannerToken Token;

  /** Provadi cyklus tak dlouho dokud neprecte vsechny instrukce nebo dokud nenastane chyba **/
  while ( (TapeIndex < InsTape->count) && error == 0 ) {
    ActInst = &(InsTape->insts[TapeIndex++]);  // Aktulane zpracovavana instrukces
  
#ifndef NDEBUG
    if ( ActInst == NULL ) {
      fprintf ( stderr, "Internal error RuntimeProces, read from tape\n");
      error = 1;
    }
#endif

    switch (ActInst->Inst) {

      /* Instrukce inPUSHV, na vrchol zasobniku ulozi
       *  strukturu s ukazatelem na data a typem dat
       *  ukazatel na data ziska z tabulky promenych */
      case inPUSHV: {
        /* Kontrola zda promena neni nahodou navesti */
	if ( SymbolsGetType((PSymbol)ActInst->Params.pointer) == stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  illegal use label: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}

	/* Nejdrive se z tabulky konstant vytahne ukazatel ne promenou */
        PVariable var = SymbolsGetVariable(ActInst->Params.pointer);
        ActType = VariablesGetType(var); // potom je teprve mozne dostat typ 
        ActValue = VariablesGetValue(var); // a data

#ifndef NDEBUG
	if (ActValue == NULL && ActType != vtUninit ){
          fprintf ( stderr, "Internal error RuntimeProces, variable value PUSHV\n" );
          exit ( 1 );
	}
#endif
	/* Data se zkopiruji a na zasobnik se ulozi odkaz s typem dat */
	switch( ActType ){

          case vtInt: {
            int* i = CopyInt( ActValue );
            StackPush(&Stack, vtInt, i);
          } break;

          case vtFloat: {
            double* d = CopyFloat( ActValue );
            StackPush(&Stack, vtFloat, d);
         } break;

          case vtString: {
            char* string = CopyString( ActValue, strlen(ActValue) );
            StackPush(&Stack, vtString, string);
         } break;

	  case vtUninit: {
            StackPush( &Stack, vtUninit, NULL );
         } break;

	  default: {
            fprintf(stderr, "Internal error RuntimeProces, PUSHV unknow type\n");
	    free ( ActValue );
	    error = 1;
         } break;
       }
      } break;
 
      /* Instrukce inPUSHC, na vrchol zasobniku ulozi
      *  strukturu s ukazatelem na data a typem dat */
      case inPUSHC: {
 	/* Kontrola zda nema pracovat s navsetim */
	if ( SymbolsGetType((PSymbol)ActInst->Params.pointer) == stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  illegal use label: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}
        PVariable var = ActInst->Params.pointer; // tady se pracuje s konstantou
        ActType = VariablesGetType(var);    // typ
        ActValue = VariablesGetValue(var); // a data jsou primo v ni a nemusi se hledat v tabulce

#ifndef NDEBUG
	if (ActValue == NULL ){
          fprintf ( stderr, "Internal error RuntimeProces, variable value PUSHC\n" );
          error = 1;
	}
#endif
	/* Data se zkopiruji a a na zasobnuik se ulozi odkaz a typ data */
	switch(ActType ){

          case vtInt: {
            int* i = CopyInt ( ActValue );
            StackPush(&Stack, vtInt, i);
         } break;
		  
          case vtFloat: {
            double* d = CopyFloat ( ActValue );
            StackPush( &Stack, vtFloat, d );
         } break;
		    
          case vtString: {
            char* string = CopyString ( ActValue, strlen(ActValue) );
            StackPush( &Stack, vtString, string );
         } break;

	  case vtUninit: {
            StackPush( &Stack, vtUninit, NULL ); 
         } break;
 
 	  default: {
            fprintf(stderr, "Internal error RuntimeProces, PUSHC unknow type\n");
            free ( ActValue );
	    error = 1;
         } break;
       }

      } break;
	
      /* Instrukce inPOPV, vybere ze zasobniku ukazatel na data
       * a jejich typ, to ulozi do promene */
      case inPOPV: {
        StackPop (&Stack, &ActType, &ActValue);
	/* Kontrola zda nse se po nas nechce aby sme ukladaly do navesti */
	if ( SymbolsGetType((PSymbol)ActInst->Params.pointer) == stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  redefine label: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
          free(ActValue);
	  break;
	}

        PVariable var = SymbolsGetVariable(ActInst->Params.pointer);
	/* Ukazatel na data se vybral ze zasboniku a preda se funkci ktera je ulozi ke 
	   konkretni promenne */
        switch (ActType){
             case vtInt: {
	       VariablesStore ( var, vtInt, ActValue);
             } break;
 
             case vtFloat: {
	       VariablesStore ( var, vtFloat, ActValue );
             } break;

             case vtString: {
	       VariablesStore ( var, vtString,  ActValue);
             } break;

	     case vtUninit: {		     
	       VariablesStore ( var, vtUninit,  ActValue);
             } break;

             default: {
               GlobalErr("%s:  %i:  Runtime error:  internal\n", FileName,  ActInst->line_no + 1);
	       error = 1;
	       free(ActValue);
             } break;
          }
      } break;

/********************** OPERACE READV ************************/
      case inREADV: {
	/* Kontorla zda nema ulkadat do navesti */
        if ( SymbolsGetType ( (PSymbol)ActInst->Params.pointer ) == stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  redefine label: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}

        PVariable var = SymbolsGetVariable(ActInst->Params.pointer); 
	ScannerGetNextToken(Scanner,&Token);

	   switch (Token.tType){
             case tLitInt: {
	       ActValue = VariablesGetValue(Token.pData);
               int* i = CopyInt( ActValue );
               VariablesStore (var, vtInt, i);
             } break;

	     case tLitFloat: {
               ActValue = VariablesGetValue(Token.pData);
               double* d = CopyFloat( ActValue );
	       VariablesStore (var, vtFloat, d);
             } break;

	     case tLitString: {
	       ActValue = VariablesGetValue(Token.pData);
               char* string = CopyString( ActValue, strlen(ActValue) );
	       VariablesStore (var, vtString, string);
             } break;

	     default: {
               GlobalErr("%s:  %i:  Runtime error:  read from stdin\n", FileName, ActInst->line_no + 1);
	       error = 1;
             } break;
           }
         } break;

/****************** OPERACE PRINTV *********************/      
      case inPRINTV: {
	/* Kontorla zda nema vypisovat navesti */
        if ( SymbolsGetType ( (PSymbol)ActInst->Params.pointer ) == stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  print label : %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}

        PVariable var = SymbolsGetVariable(ActInst->Params.pointer); 
        ActType = VariablesGetType(var);
        ActValue = VariablesGetValue(var);

	switch ( ActType ) {

          case vtInt: {
	    int * val = ActValue;
            printf("%d", *val);
          } break;

          case vtFloat: {
	    double* val = ActValue;
            printf("%g", *val);
          } break;

          case vtString: {
	    char* val = ActValue;
            printf("%s", val);
          } break;

          case vtUninit: {
             GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in function print.\n", FileName, ActInst->line_no + 1);
             error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type in function print.\n", FileName, ActInst->line_no + 1+1);
            error = 1;
          } break;
        }
      } break;

/****************** NEPODMINENY SKOK ****************************/      
      case inJMPV: {

        if ( SymbolsGetType((PSymbol)ActInst->Params.pointer) != stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  illegal jump on variable: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}

        TapeIndex = SymbolsGetLabelIndex((PSymbol)ActInst->Params.pointer);
      } break;

/***************** OPERACE PODMINEHO SKOKU *********************/      
      case inJNZV: {
        StackPop (&Stack, &ActType, &ActValue);

        if ( SymbolsGetType((PSymbol)ActInst->Params.pointer) != stLabel) {
          GlobalErr ("%s:  %i:  Runtime error:  illegal jump on variable: %s \n", FileName, ActInst->line_no + 1, SymbolsGetName((PSymbol)ActInst->Params.pointer));
	  error = 1;
	  break;
	}

	switch (ActType){
             case vtInt: {
	       if ( 0 != *(int*)ActValue ) 
                 TapeIndex =  SymbolsGetLabelIndex((PSymbol)ActInst->Params.pointer);  
             } break;

             case vtFloat: {	    
	       if ( 0.0 != *(double*)ActValue ) 
                 TapeIndex =  SymbolsGetLabelIndex((PSymbol)ActInst->Params.pointer);  
	     } break;

             case vtString: {		     
	       if ( 0 != strlen( (char*)ActValue ) ) 
                 TapeIndex =  SymbolsGetLabelIndex((PSymbol)ActInst->Params.pointer);  
	     } break;

	     case vtUninit: {		     
               GlobalErr("%s:  %i:  Runtime error: jump with uninitialized variable.\n", FileName,  ActInst->line_no + 1);
	       error = 1;
	     } break;

             default: {
               GlobalErr("%s:  %i:  Runtime error: intern, unknow type.\n", FileName,  ActInst->line_no + 1);
	       error = 1;
	     } break;
          } 

        free(ActValue);
      } break;

/********************** OPERACE SCITANI **************************/
      case inADD: {
        StackPop (&Stack, &SecondType, &SecondValue); 
        StackPop (&Stack, &ActType, &ActValue);

	switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
	        int* tmp =(int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) + (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
		double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(int*)ActValue) + *(double*)SecondValue;
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtString: {
                sprintf (Temp,"%d",*(int*)ActValue);

		char* tmp = CopyString(Temp, strlen((char*)SecondValue)+strlen(Temp));

                strcat(tmp, (char*)SecondValue );

                StackPush(&Stack, vtString, tmp);
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
		double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) + (*(int*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtFloat: {
		double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) + (*(double*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtString: {
                sprintf (Temp,"%g",*(double*)ActValue );

		char* tmp = CopyString(Temp, strlen((char*)SecondValue) + strlen(Temp));

                strcat(tmp, (char*)SecondValue );

                StackPush(&Stack, vtString, tmp);
	      } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtString: {

            switch ( SecondType ) {
 
              case vtInt: {
                sprintf (Temp,"%d",*(int*)SecondValue);

		char* tmp = CopyString(ActValue, strlen((char*)ActValue) + strlen(Temp));

                strcat(tmp, Temp );

                StackPush(&Stack, vtString, tmp);
              } break;

              case vtFloat: {
                sprintf (Temp,"%g",*(double*)SecondValue);

		char* tmp = CopyString(ActValue, strlen((char*)ActValue) + strlen(Temp));

                strcat(tmp, Temp);

                StackPush(&Stack, vtString, tmp);
              } break;

              case vtString: {
       		char* tmp = CopyString(ActValue, strlen((char*)SecondValue) + strlen((char*)ActValue));

                strcat(tmp, (char*)SecondValue);

                StackPush(&Stack, vtString, tmp);
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; // END ADD

/************************* Operace odcitani ***************************/
      case inSUB: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
	        int* tmp = (int*) RuntimeMalloc(sizeof(int));

                *tmp = (*(int*)ActValue) - (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(int*)ActValue) - (*(double*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error: bad operation with string.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error: uninitialized variable.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error: unknow variable type.", FileName, ActInst->line_no + 1);
                error = 1;
              } break; 
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) - (*(int*)SecondValue);		
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtFloat: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) - (*(double*)SecondValue);		
		StackPush(&Stack, vtFloat , tmp);
             } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error: bad operation with string.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtString: {
             GlobalErr("%s:  %i:  Runtime error: bad operation with string.", FileName, ActInst->line_no + 1);
             error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
     free(ActValue);
     free(SecondValue);
     } break; // END SUB

/*************************** Operace NASOBENI *************************************/
    case inMUL: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp =(int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) * (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(int*)ActValue) * (*(double*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  bad operation with string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) * (*(int*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtFloat: {
                double* tmp =(double*) RuntimeMalloc(sizeof(double));

		*tmp = (*(double*)ActValue) * (*(double*)SecondValue);
                StackPush(&Stack, vtFloat , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  bad operation with string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtString: {
             GlobalErr("%s:  %i:  Runtime error:  bad operation with string.\n", FileName, ActInst->line_no + 1);
             error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
     free(ActValue);
     free(SecondValue);
     } break; // END MUL

/**************************** Operace DELENI **************************************/
    case inDIV: {
      StackPop (&Stack, &SecondType, &SecondValue);
      StackPop (&Stack, &ActType, &ActValue);

	switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
	        if ( 0 != (*(int*)SecondValue) ) { // vzdy kontroluje zda nedeli nulou
		  int* tmp =(int*) RuntimeMalloc(sizeof(int));

		  *tmp = (*(int*)ActValue) / (*(int*)SecondValue);
		  StackPush(&Stack, vtInt , tmp);
		}
		else {
	          GlobalErr("%s:  %i:  Runtime error:  integer divided by zero.\n", FileName, ActInst->line_no + 1);
		  error = 1;
		}
              } break;

              case vtFloat: {
	        if ( 0.0 != *(double*)SecondValue ) {
		  double* tmp =(double*) RuntimeMalloc(sizeof(double));

		  *tmp = (*(int*)ActValue) / (*(double*)SecondValue);
		  StackPush(&Stack, vtFloat , tmp);
		}
		else {
	          GlobalErr("%s:  %i:  Runtime error:  double divided by zero.\n", FileName, ActInst->line_no + 1);
		  error = 1;
		}
	      } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error: bad operation with string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
       	        if ( 0 != (*(int*)SecondValue) ) {
		  double* tmp =(double*) RuntimeMalloc(sizeof(double));

		  *tmp = (*(double*)ActValue) / (*(int*)SecondValue);
		  StackPush(&Stack, vtFloat , tmp);
		}
		else {
	          GlobalErr("%s:  %i:  Runtime error:  double divided by zero.\n", FileName, ActInst->line_no + 1);
		  error = 1;
		}
              } break;

              case vtFloat: {
 	        if ( 0.0 != *(double*) SecondValue ) {			
		  double* tmp =(double*) RuntimeMalloc(sizeof(double));

		  *tmp = *(double*)ActValue / *(double*)SecondValue;
		  StackPush(&Stack, vtFloat , tmp);
		}
		else {
	          GlobalErr("%s:  %i:  Runtime error:  double divided by zero.\n", FileName, ActInst->line_no + 1);
		  error = 1;
		}
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error: bad operation with string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  bad operation with string\n", FileName, ActInst->line_no + 1);
                error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
     free(ActValue);
     free(SecondValue);
     } break; // END DIV

/***************************** POROVNAVANI ************************************/
      case inCMPE: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);
	
        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) == (*(int*)SecondValue);
                StackPush(&Stack, vtInt, tmp);
              } break;

              case vtFloat: {
                int* tmp =  (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) == (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare integer vith string\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error: unknow type of variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) == (*(int*)SecondValue);
		StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) == (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow type of variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtString: {

            switch ( SecondType ) {

              case vtInt: {
                GlobalErr("%s:  %i:  Runtime error:  comparing integer vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtFloat: {
                GlobalErr("%s:  %i:  Runtime error:  comparing float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtString: { // muze porovnat dva retezce
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		if (0 == strcmp((char*)ActValue, (char*)SecondValue))
                  *tmp = 1;
                else 
                  *tmp = 0;

                StackPush(&Stack, vtInt, tmp);
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow type of variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow type of variable.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; 
      /**** END CMPE ****/
 
      case inCMPNE: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) != (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) != (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare integer vith string\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) != (*(int*)SecondValue);
		StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) != (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare double vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable\n.", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtString: {

            switch ( SecondType ) {
 
              case vtInt: {
                GlobalErr("%s:  %i:  Runtime error: compare integer vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
 
              case vtFloat: {
                GlobalErr("%s:  %i:  Runtime error: compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtString: { // muze porovnat dva retezce
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

                if (0 == strcmp((char*)ActValue, (char*)SecondValue))
                  *tmp = 0;
                else 
                  *tmp = 1;

                StackPush(&Stack, vtInt, tmp);
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error: uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error: unknow variable type in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }
            }
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; // END CMPNE

    case inCMPG: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) > (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) > (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare integer vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }  
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp =(int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) > (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) > (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              }
            }
          } break;

          case vtString: {
            GlobalErr("%s:  %i:  Runtime error: bad string comparing.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; // END CMPG

      case inCMPL: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) < (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) < (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  comparing integer with string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  comparing uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  comparing unknow variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) < (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) < (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtString: {
            GlobalErr("%s:  %i:  Runtime error: bad string comparing.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }

      free(ActValue);
      free(SecondValue);
      } break; // END CMPL

      case inCMPLE: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) <= (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) <= (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare int vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = *(double*)ActValue <= (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;
 
              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = *(double*)ActValue <= *(double*)SecondValue;
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtString: {
            GlobalErr("%s:  %i:  Runtime error: bad string comparing.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; // END CMPLE

      case inCMPGE: {
        StackPop (&Stack, &SecondType, &SecondValue);
        StackPop (&Stack, &ActType, &ActValue);

        switch (ActType) {

          case vtInt: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) >= (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(int*)ActValue) >= (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare int vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtFloat: {

            switch ( SecondType ) {

              case vtInt: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) >= (*(int*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtFloat: {
                int* tmp = (int*) RuntimeMalloc(sizeof(int));

		*tmp = (*(double*)ActValue) >= (*(double*)SecondValue);
                StackPush(&Stack, vtInt , tmp);
              } break;

              case vtString: {
                GlobalErr("%s:  %i:  Runtime error:  compare float vith string.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              case vtUninit: {
                GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare .\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;

              default : {
                GlobalErr("%s:  %i:  Runtime error:  unknow variable type in compare.\n", FileName, ActInst->line_no + 1);
                error = 1;
              } break;
            }
          } break;

          case vtString: {
            GlobalErr("%s:  %i:  Runtime error:  bad string comparing.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          case vtUninit: {
            GlobalErr("%s:  %i:  Runtime error:  uninitialized variable in compare.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;

          default : {
            GlobalErr("%s:  %i:  Runtime error:  unknow variable type.\n", FileName, ActInst->line_no + 1);
            error = 1;
          } break;
        }
      free(ActValue);
      free(SecondValue);
      } break; // END CMPGE


      case inSORT: {
        int  NoParams = ActInst->Params.number;
        char**  array = (char**) malloc ( sizeof(char*) * NoParams);

	if ( array == NULL ){
	  fprintf (stderr, "error: not enought memory\n");
	  exit ( 1 );
	}

#ifndef NDEBUG
        if ( NoParams == 0){ 
	  fprintf (stderr, "Error 0 params for Heapsrot\n");
	  exit ( 1 );
        }
#endif

        for (int i = 0; i < NoParams; i++ ) {
	  StackPop ( &Stack, &ActType, &ActValue );

            switch ( ActType ) {
	      case vtInt : {
	        int* integer = ActValue;
   	        sprintf(Temp, "%d", *integer);

	        char* string = CopyString(Temp, strlen(Temp));
	        array[i] = string;
	      } break;

 	      case vtFloat : {
	        double* d = ActValue;
	        sprintf(Temp, "%g", *d);

	        char* string = CopyString(Temp, strlen(Temp));
	        array[i] = string;
	      } break;
 
 	      case vtString : {
	        char* string = CopyString(ActValue, strlen(ActValue));
	        array[i] = string;
	      } break;

 	      case vtUninit : {
	        GlobalErr("%s:  %i:  Runtime error:  bad argument in Heapsort\n", FileName, ActInst->line_no + 1 );
	        error = 1;
	      } break;
	    } // switch
        free( ActValue );
        } // for
      char* sortedString = heapsort(array, NoParams);
      StackPush (&Stack, vtString, sortedString);
      free(array);
    } break; // ## END Sort ##
    } // switch
  } // while

  StackDestroy(&Stack);
  ScannerDestroy(Scanner);

  return error;
}
