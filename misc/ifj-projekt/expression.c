/*
 * @description   Syntaktick� anal�za v�raz� (zdola nahoru)
 * @author        Vendula Poncov� - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "expression.h"

// tisknout pomocn� v�pisy:
#define TISK 0
#if TISK
#define tisk(prikazy) prikazy
#else
#define tisk(prikazy)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// preceden�n� tabulka:
const char precedentTable[MAXTAB][MAXTAB] = {

// tokeny:                  id   (   )   +   -   *   /   ^  ..   <  <=   >  >=  ==  ~=             $
   [L_ID]            ={[L_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_LEFT_BRACKET]  ={[L_ID]='<','<','=','<','<','<','<','<','<','<','<','<','<','<','<',[ENDEXPR]= 0 },
   [L_RIGHT_BRACKET] ={[L_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_ADDITION]      ={[L_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_SUBTRACTION]   ={[L_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_MULTIPLICATION]={[L_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_DIVISION]      ={[L_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_POWER]         ={[L_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_CONCATENATION] ={[L_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_SMALLER]       ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_SMALLER_EQUAL] ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_BIGGER]        ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_BIGGER_EQUAL]  ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_EQUAL]         ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [L_UNEQUAL]       ={[L_ID]='<','<','>','<','<','<','<','<','<','>','>','>','>','>','>',[ENDEXPR]='>'},
   [ENDEXPR]         ={[L_ID]='<','<', 0 ,'<','<','<','<','<','<','<','<','<','<','<','<',[ENDEXPR]='$'},

}; //precedentTable


// tabulka pro kontrolu semantiky :
const int semTable[][DATTYPE] = {

   // NIL,BOOL,NUMBER,STRING
   // 7
   [I_ADD]   ={0,0,1,0},
   [I_SUB]   ={0,0,1,0},
   [I_MUL]   ={0,0,1,0},
   [I_DIV]   ={0,0,1,0},
   [I_POW]   ={0,0,1,0},
   [I_CON]   ={0,0,0,1},
   [I_CMP_L] ={0,0,1,1},
   [I_CMP_LE]={0,0,1,1},
   [I_CMP_G] ={0,0,1,1},
   [I_CMP_GE]={0,0,1,1},
   [I_CMP_E] ={1,1,1,1},
   [I_CMP_NE]={1,1,1,1},
   // 18
}; // semTable


TStack Stack;

////////////////////////////////////////////////////////////////////////  Funkce parseExpression()

/*
 * Syntaktick� a s�matick� kontrola v�razu pomoc�
 * preceden�n� SA a z�sobn�ku.
 * Prvn� token, kter� by zp�sobil syntaktickou chybu je
 * pova�ov�n za konec v�razu a bude zpracov�n parserem.
 * S�mantika kontroluje definov�n� prom�nn�ch a operace
 * s konstantami.
 * Ukazatel *ptrResult bude ukazovat na v�sledek v�razu.
 * @param   ukazatel na tabulku
 * @param   ukazatel na adresu v�sledn� prom�nn�
 * @return  chybov� k�d
 */
int parseExpression(TTable *table, TVar **ptrResult) {

   // inicializace :
   int err = EOK;

   TList *LTmpVars = &table->lastAddedFunc->tmpVar;
   listFirst(LTmpVars);

   stackInit(&Stack);
   err = shift(&Stack, ENDEXPR, NULL);      // vlo�eno dno z�sobn�ku

   TInstr instr = {NOINSTR, NULL, NULL, NULL};
   int a, b;
   char c;

   // syntaktick� anal�za v�razu: 

   do {
      a = token;                        // aktu�ln� token
      b = getTopTerminal(&Stack);       // najdeme nejvrchn�j�� termin�l

      if (isConst(a) || isId(a)) {      // pokud je to konstanta nebo identifik�tor
         c = precedentTable[b][L_ID];   // kontrolujeme syntaxi podle identifik�toru

         // pokud je na vrcholu z�sobn�ku v�raz, do�lo k chyb�
         if (getTopToken(&Stack) == EXPRESSION) c = 0;
      }
      else {
         c = precedentTable[b][a];      // pod�v�me se do tabulky
      }

      if (c == 0) {                     // chyba
         a = ENDEXPR;                   // token pova�ujeme za konec v�razu
         c = precedentTable[b][a];      // znovu se pod�v�me do tabulky
      }

      switch(c) {

      case '=':
      case '<':
         err = shift(&Stack, a, NULL);                // push(a)
         if (err != EOK) break;

         token = getNextToken(&attr);                 // next token
         if (token < 0) err = token;
         break;

      case '>':
         err = findRule(&Stack, &instr);              // najdi pravidlo
         if (err != EOK) break;

         err = checkRule(&instr);                     // kontrola s�mantiky
         if (err != EOK) break;

         err = insertInstruction(&instr, table);      // vlo� instrukci
         if (err != EOK) break;

         err = shift(&Stack, EXPRESSION, instr.dest); // push(v�sledek)
         break;

      case '$':
         err = returnResult(&Stack, ptrResult);       // ukazatel na v�sledek
         break;

      default :
         err = SYN_ERR;                               // syntaktick� chyba
      };

      // kontrolni vypis:
      tisk(
         printf("\n a = %d b = %d c = %c err = %d \n\n", a, b, c, err);
         tiskniStack(&Stack);
      )

   } while ((a != ENDEXPR || b != ENDEXPR) && err == EOK);

   // kontrolni vypis:
   tisk(
      printf("vysledek = %d\n", (int) *ptrResult);
   )

   // �klid
   stackDataDelete(&Stack);
   stackDelete(&Stack);

   return err;
}

////////////////////////////////////////////////////////////////////////  Dal�� funkce

/*
 * Operace nad z�sobn�kem: SHIFT
 * Na z�sobn�k se vlo�� aktu�ln� token a p��padn� uk na TVar.
 * Pokud identifik�tor: vyhled� se v tabulce symbol�,
 * pokud konstanta:     vlo�� se do tabulky symbol�,
 * p��padn� ukazatel p�ed�n v parametru pom.
 * @param   ukazatel na z�sobn�k
 * @param   aktu�ln� token
 * @param   ukazatel na TVar
 * @return  chybov� k�d
 */
int shift (TStack *S, int token, TVar *pom) {

   int err = EOK;

   // inicialiazce dat:

   // identifik�tor
   if (isId(token)) {
      pom = functionSearchVar(table->lastAddedFunc, attr);
      if (pom == NULL)
         err = SEM_ERR; // nedefinovan� prom�nn�
      token = EXPRESSION;             // pravidlo E->id
   }

   // konstanta
   else if (isConst(token)) {
      if (functionInsertConstatnt(table->lastAddedFunc, attr, token) == INS_OK) {
         pom = (TVar*) listCopyLast(&table->lastAddedFunc->constants);
      } else err = INTR_ERR;          // nedostatek pam�ti
      token = EXPRESSION;             // pravidlo E->const
   }

   // vlo��me data na z�sobn�k :
   if (err == EOK) {
      TStackData *data = createStackData(token, pom, &err);
      if (err == EOK) err = stackPush(S, data);
   }

   return err;
}

/*
 * Operace nad z�sobn�kem: GET TOP TERMINAL
 * Vr�t� hodnotu tokenu nejvrchn�j��ho termin�lu na z�sobn�ku.
 * P�edpokl�d� se, �e na dn� z�sobn�ku je termin�l ENDEXPR.
 * Se z�sobn�kem se nepracuje pomoc� fc� stack.h !
 * @param   ukazatel na z�sobn�k
 * @return  token
 */
int getTopTerminal(TStack *S) {

   TSItem *pom = S->top;

   // hled�me termin�l nejbl�e k z�sobn�ku:
   while (pom != NULL && ((TStackData*)pom->data)->token == EXPRESSION) {
      pom = pom->next;
   }

   if (pom == NULL) return -1;
   else return ((TStackData*)pom->data)->token;
}

/*
 * Operace nad z�sobn�kem: GET TOP TOKEN
 * Vr�t� hodnotu tokenu na vrcholu z�sobn�ku.
 * @param   ukazatel na z�sobn�k
 * @return  token
 */
int getTopToken (TStack *S) {
  return ((TStackData*)stackTop(S))->token;
}

/*
 * Operace nad z�sobn�kem: REDUCTION
 * Odeb�r� ze z�sobn�ku tokeny a termin�ly a hled�
 * pro n� pravidlo.
 * Do prom�nn� instr se generuje instrukce.
 * @param   ukazatel na z�sobn�k
 * @param   ukazatel na instrukci
 * @return  chybov� k�d
 */
int findRule(TStack *S, TInstr *instr) {

   int err = SYN_ERR;
   TStackData *top = NULL;

   if (!stackEmpty(S)) {
      top = (TStackData *) stackTopPop(S);

      // pravidlo E -> (E)
      if (top->token == L_RIGHT_BRACKET) {         // pop(prav� z�vorka)
         instr->type = NOINSTR;                     // nebude se generovat pravidlo
         free(top);
         if (!stackEmpty(S)) {
            top = (TStackData *) stackTopPop(S);

            if (top->token == EXPRESSION) {          // pop(v�raz)
               instr->dest = top->var;
               free(top);
               if (!stackEmpty(S)) {
                  top = (TStackData *) stackTopPop(S);

                  if (top->token == L_LEFT_BRACKET) {  // pop(lev� z�vorka)
                     err = EOK;                         // nalezeno pravidlo!
                     free(top);
                  }
               }
            }
         }
      } // konec pravidla

      // pravidlo E -> E op E
      else if (top->token == EXPRESSION) {        // pop(v�raz)
         instr->src2 = top->var;
         free(top);
         if (!stackEmpty(S)) {
            top = (TStackData *) stackTopPop(S);

            if (isOperator(top->token)) {           // pop(oper�tor)
               instr->type = top->token + OFFSET;
               free(top);
               if (!stackEmpty(S)) {
                  top = (TStackData *) stackTopPop(S);

                  if (top->token == EXPRESSION) {    // pop(v�raz)
                     err = EOK;                       // nalezeno pravidlo!
                     instr->src1 = top->var;
                     free(top);
                  }
               }
            }
         }
      } // konec pravidla
   }

   // pokud n�kde zhavarovalo
   if (err != EOK && top != NULL) {
      free(top);
   }

   return err;
}

/*
 * Provede s�mantickou kontrolu instrukce.
 * Kontrola se t�k� pouze konstant.
 * @param   ukazatel na instrukci
 * @return  chybov� k�d
 */
int checkRule (TInstr *instr) {

   // pravidlo E -> (E)
   if (instr->type == NOINSTR) return EOK;

   // pravidlo E -> E op E
   int err = EOK;
   int cntConst = 0;
   TVar *src1 = (TVar*)instr->src1;
   TVar *src2 = (TVar*)instr->src2;

   // kontrola pro lev� operand a operaci
   if (src1->varType == VT_CONST) {
      cntConst++;
      err = checkSemErr(instr, src1->varData);
   }

   // kontrola pro prav� operand a operaci
   if (err == EOK && src2->varType == VT_CONST) {
      cntConst++;
      err = checkSemErr(instr, src2->varData);
   }

   // kontrola obou operand�
   if (err == EOK && cntConst == 2) {
      if (isLGEOperation(instr->type) &&
            (src1->varData->type != src2->varData->type)) {
         err = SEM_ERR;
      }
   }

   return err;
}

/*
 * Vlo�� kopii instrukce do seznamu instrukc�.
 * Vygeneruje pomocnou prom�nou a pou�ije ji
 * jako destination instrukce.
 * @param   ukazatel na instrukci
 * @param   ukazatel na tabulku
 * @return  chybov� k�d
 */
int insertInstruction(TInstr *instr, TTable *table) {

   // pravidlo E -> (E)
   if (instr->type == NOINSTR) return EOK;

   // pravidlo E -> E op E
   int err = EOK;
   TList *LTmpVars = &table->lastAddedFunc->tmpVar;
   TList *LInstr = &table->lastAddedFunc->instructions;

   // vytvo� pomocnou prom�nnou pro v�sledek operace
   TVar *var = createTmpVar(LTmpVars, &err);
   if (err == EOK) {
      instr->dest = var;

      // vytvo� novou instrukci
      TInstr *newInstr = genInstr(instr->type, instr->dest, instr->src1, instr->src2);
      if (newInstr != NULL) {

         // vlo� instrukci do seznamu
         err = listInsertLast(LInstr, newInstr);
      } else err = INTR_ERR; // nedostatek pam�ti
   }

   return err;
}

/*
 * Operace nad z�sobn�kem: POP RESULT
 * Ze z�sobn�ku vyjme v�sledek v�razu a
 * nastav� na n�j ukazatel result.
 * @param   ukazatel na z�sobn�k
 * @param   ukazatel na adresu v�sledku
 * @return  chybov� k�d
 */
int returnResult(TStack *S, TVar **ptrResult) {

   int err = SYN_ERR;
   TStackData *top = NULL;

   if (!stackEmpty(S)) {
      top = (TStackData *) stackTopPop(S);
      if (top->token == EXPRESSION) {

         // ze stacku se poda�ilo vyjmout v�raz
         *ptrResult = top->var;
         err = EOK;
      }
      free(top);
   }

   return err;
}

////////////////////////////////////////////////////////////////////////  Vytv��en� a maz�n� pomocn�ch struktur

/*
 * Vytvo�� pomocnou prom�nnou a vr�t� na ni ukazatel.
 * Prom�nn� se pro ka�d� v�raz "recykluj�" :
 * Pro dal�� v�raz se pou�ij� tyt� prom�nn�, pokud dojdou,
 * vlo�� se do seznamu nov� prom�nn�. �e�eno pomoc� aktivn�ho prvku.
 * @param   ukazatel na seznam pomocn�ch prom�nn�ch
 * @param   ukazatel na chybu
 * @return  ukazatel na vytvo�enou prom�nnou
 */
TVar *createTmpVar(TList *L, int *err) {

   // posuneme aktivitu na dal�� prvek seznamu :
   listSucc(L);

   // aktivita se ztratila, je t�eba vytvo�it novou prom�nnou
   if (!listActive(L)) {

      // alokace prom�nn� a dat
      TVar *pom = NULL;
      if ( (pom = malloc(sizeof(TVar))) != NULL) {
         if ( (pom->varData = (TVarData*)malloc(sizeof(TVarData))) != NULL ) {

            pom->varType = VT_TMP_VAR;        // inicializace
            pom->name = NULL;
            pom->varData->type = NIL;
            *err = listInsertLast(L, pom);    // vlo�en� do seznamu
            listLast(L);                      // stane se aktivn�m
         } else {
            *err = INTR_ERR;                  // chyba p�i alokaci
            free(pom);
         }
      } else *err = INTR_ERR;
   }

   //  vr�t� ukazatel na prom�nnou nebo NULL
   if (*err == EOK) {
      return (TVar*)listGetActive(L)->data;
   } else return NULL;
}

/*
 * Vytvo�� a inicializuje data pro z�sobn�k.
 * @param   token
 * @param   ukazatel na prom�nnou
 * @param   ukazatel na chybu
 * @return  ukazatel na vytvo�en� data
 */
TStackData *createStackData(int token, TVar *var, int *err) {

   TStackData *data = NULL;
   if ( (data = (TStackData*)malloc(sizeof(TStackData))) != NULL ) {
      data->token = token;
      data->var = var;
   } else *err = INTR_ERR; // nedostatek pam�ti

   return data;
}

/*
 * Smaz�n� v�ech dat v z�sobn�ku.
 * @param   ukazatel na z�sobn�k
 */
void stackDataDelete(TStack *S) {

   TSItem *pom = S->top;
   while (pom != NULL) {
      free (pom->data);
      pom = pom->next;
   }
}

////////////////////////////////////////////////////////////////////////  Funkce pro kontroln� v�pisy

/*
 * Vytiskne preceden�n� tabulku
 */
void tiskniPrecTab() {
   printf("%d %d", MAXTAB, ENDEXPR);
   printf("\n");
   for (int i = 0; i<MAXTAB; i++) {
      for (int j = 0; j<MAXTAB; j++) {
         if (precedentTable[i][j] == 0) printf("0 ");
         else printf("%c ",precedentTable[i][j]);
      }
      printf("\n");
   }
}

/*
 * Vytiskne obsah z�sobn�ku
 */
void tiskniStack (TStack *s) {

   printf("Stav z�sobn�ku: top\n");
   TSItem *pom = s->top;
   while (pom != NULL) {
      printf("\t%d\n",((TStackData*)(pom->data))->token);
      pom = pom->next;
   }
   printf("_______bottom_________\n\n");
}

/* konec expression.c */
