/*
 * @description   Syntaktická analýza výrazù (zdola nahoru)
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#include "expression.h"

// tisknout pomocné výpisy:
#define TISK 0
#if TISK
#define tisk(prikazy) prikazy
#else
#define tisk(prikazy)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// precedenèní tabulka:
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
 * Syntaktická a sématická kontrola výrazu pomocí
 * precedenèní SA a zásobníku.
 * První token, který by zpùsobil syntaktickou chybu je
 * pova¾ován za konec výrazu a bude zpracován parserem.
 * Sémantika kontroluje definování promìnných a operace
 * s konstantami.
 * Ukazatel *ptrResult bude ukazovat na výsledek výrazu.
 * @param   ukazatel na tabulku
 * @param   ukazatel na adresu výsledné promìnné
 * @return  chybový kód
 */
int parseExpression(TTable *table, TVar **ptrResult) {

   // inicializace :
   int err = EOK;

   TList *LTmpVars = &table->lastAddedFunc->tmpVar;
   listFirst(LTmpVars);

   stackInit(&Stack);
   err = shift(&Stack, ENDEXPR, NULL);      // vlo¾eno dno zásobníku

   TInstr instr = {NOINSTR, NULL, NULL, NULL};
   int a, b;
   char c;

   // syntaktická analýza výrazu: 

   do {
      a = token;                        // aktuální token
      b = getTopTerminal(&Stack);       // najdeme nejvrchnìj¹í terminál

      if (isConst(a) || isId(a)) {      // pokud je to konstanta nebo identifikátor
         c = precedentTable[b][L_ID];   // kontrolujeme syntaxi podle identifikátoru

         // pokud je na vrcholu zásobníku výraz, do¹lo k chybì
         if (getTopToken(&Stack) == EXPRESSION) c = 0;
      }
      else {
         c = precedentTable[b][a];      // podíváme se do tabulky
      }

      if (c == 0) {                     // chyba
         a = ENDEXPR;                   // token pova¾ujeme za konec výrazu
         c = precedentTable[b][a];      // znovu se podíváme do tabulky
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

         err = checkRule(&instr);                     // kontrola sémantiky
         if (err != EOK) break;

         err = insertInstruction(&instr, table);      // vlo¾ instrukci
         if (err != EOK) break;

         err = shift(&Stack, EXPRESSION, instr.dest); // push(výsledek)
         break;

      case '$':
         err = returnResult(&Stack, ptrResult);       // ukazatel na výsledek
         break;

      default :
         err = SYN_ERR;                               // syntaktická chyba
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

   // úklid
   stackDataDelete(&Stack);
   stackDelete(&Stack);

   return err;
}

////////////////////////////////////////////////////////////////////////  Dal¹í funkce

/*
 * Operace nad zásobníkem: SHIFT
 * Na zásobník se vlo¾í aktuální token a pøípadnì uk na TVar.
 * Pokud identifikátor: vyhledá se v tabulce symbolù,
 * pokud konstanta:     vlo¾í se do tabulky symbolù,
 * pøípadnì ukazatel pøedán v parametru pom.
 * @param   ukazatel na zásobník
 * @param   aktuální token
 * @param   ukazatel na TVar
 * @return  chybový kód
 */
int shift (TStack *S, int token, TVar *pom) {

   int err = EOK;

   // inicialiazce dat:

   // identifikátor
   if (isId(token)) {
      pom = functionSearchVar(table->lastAddedFunc, attr);
      if (pom == NULL)
         err = SEM_ERR; // nedefinovaná promìnná
      token = EXPRESSION;             // pravidlo E->id
   }

   // konstanta
   else if (isConst(token)) {
      if (functionInsertConstatnt(table->lastAddedFunc, attr, token) == INS_OK) {
         pom = (TVar*) listCopyLast(&table->lastAddedFunc->constants);
      } else err = INTR_ERR;          // nedostatek pamìti
      token = EXPRESSION;             // pravidlo E->const
   }

   // vlo¾íme data na zásobník :
   if (err == EOK) {
      TStackData *data = createStackData(token, pom, &err);
      if (err == EOK) err = stackPush(S, data);
   }

   return err;
}

/*
 * Operace nad zásobníkem: GET TOP TERMINAL
 * Vrátí hodnotu tokenu nejvrchnìj¹ího terminálu na zásobníku.
 * Pøedpokládá se, ¾e na dnì zásobníku je terminál ENDEXPR.
 * Se zásobníkem se nepracuje pomocí fcí stack.h !
 * @param   ukazatel na zásobník
 * @return  token
 */
int getTopTerminal(TStack *S) {

   TSItem *pom = S->top;

   // hledáme terminál nejblí¾e k zásobníku:
   while (pom != NULL && ((TStackData*)pom->data)->token == EXPRESSION) {
      pom = pom->next;
   }

   if (pom == NULL) return -1;
   else return ((TStackData*)pom->data)->token;
}

/*
 * Operace nad zásobníkem: GET TOP TOKEN
 * Vrátí hodnotu tokenu na vrcholu zásobníku.
 * @param   ukazatel na zásobník
 * @return  token
 */
int getTopToken (TStack *S) {
  return ((TStackData*)stackTop(S))->token;
}

/*
 * Operace nad zásobníkem: REDUCTION
 * Odebírá ze zásobníku tokeny a terminály a hledá
 * pro nì pravidlo.
 * Do promìnné instr se generuje instrukce.
 * @param   ukazatel na zásobník
 * @param   ukazatel na instrukci
 * @return  chybový kód
 */
int findRule(TStack *S, TInstr *instr) {

   int err = SYN_ERR;
   TStackData *top = NULL;

   if (!stackEmpty(S)) {
      top = (TStackData *) stackTopPop(S);

      // pravidlo E -> (E)
      if (top->token == L_RIGHT_BRACKET) {         // pop(pravá závorka)
         instr->type = NOINSTR;                     // nebude se generovat pravidlo
         free(top);
         if (!stackEmpty(S)) {
            top = (TStackData *) stackTopPop(S);

            if (top->token == EXPRESSION) {          // pop(výraz)
               instr->dest = top->var;
               free(top);
               if (!stackEmpty(S)) {
                  top = (TStackData *) stackTopPop(S);

                  if (top->token == L_LEFT_BRACKET) {  // pop(levá závorka)
                     err = EOK;                         // nalezeno pravidlo!
                     free(top);
                  }
               }
            }
         }
      } // konec pravidla

      // pravidlo E -> E op E
      else if (top->token == EXPRESSION) {        // pop(výraz)
         instr->src2 = top->var;
         free(top);
         if (!stackEmpty(S)) {
            top = (TStackData *) stackTopPop(S);

            if (isOperator(top->token)) {           // pop(operátor)
               instr->type = top->token + OFFSET;
               free(top);
               if (!stackEmpty(S)) {
                  top = (TStackData *) stackTopPop(S);

                  if (top->token == EXPRESSION) {    // pop(výraz)
                     err = EOK;                       // nalezeno pravidlo!
                     instr->src1 = top->var;
                     free(top);
                  }
               }
            }
         }
      } // konec pravidla
   }

   // pokud nìkde zhavarovalo
   if (err != EOK && top != NULL) {
      free(top);
   }

   return err;
}

/*
 * Provede sémantickou kontrolu instrukce.
 * Kontrola se týká pouze konstant.
 * @param   ukazatel na instrukci
 * @return  chybový kód
 */
int checkRule (TInstr *instr) {

   // pravidlo E -> (E)
   if (instr->type == NOINSTR) return EOK;

   // pravidlo E -> E op E
   int err = EOK;
   int cntConst = 0;
   TVar *src1 = (TVar*)instr->src1;
   TVar *src2 = (TVar*)instr->src2;

   // kontrola pro levý operand a operaci
   if (src1->varType == VT_CONST) {
      cntConst++;
      err = checkSemErr(instr, src1->varData);
   }

   // kontrola pro pravý operand a operaci
   if (err == EOK && src2->varType == VT_CONST) {
      cntConst++;
      err = checkSemErr(instr, src2->varData);
   }

   // kontrola obou operandù
   if (err == EOK && cntConst == 2) {
      if (isLGEOperation(instr->type) &&
            (src1->varData->type != src2->varData->type)) {
         err = SEM_ERR;
      }
   }

   return err;
}

/*
 * Vlo¾í kopii instrukce do seznamu instrukcí.
 * Vygeneruje pomocnou promìnou a pou¾ije ji
 * jako destination instrukce.
 * @param   ukazatel na instrukci
 * @param   ukazatel na tabulku
 * @return  chybový kód
 */
int insertInstruction(TInstr *instr, TTable *table) {

   // pravidlo E -> (E)
   if (instr->type == NOINSTR) return EOK;

   // pravidlo E -> E op E
   int err = EOK;
   TList *LTmpVars = &table->lastAddedFunc->tmpVar;
   TList *LInstr = &table->lastAddedFunc->instructions;

   // vytvoø pomocnou promìnnou pro výsledek operace
   TVar *var = createTmpVar(LTmpVars, &err);
   if (err == EOK) {
      instr->dest = var;

      // vytvoø novou instrukci
      TInstr *newInstr = genInstr(instr->type, instr->dest, instr->src1, instr->src2);
      if (newInstr != NULL) {

         // vlo¾ instrukci do seznamu
         err = listInsertLast(LInstr, newInstr);
      } else err = INTR_ERR; // nedostatek pamìti
   }

   return err;
}

/*
 * Operace nad zásobníkem: POP RESULT
 * Ze zásobníku vyjme výsledek výrazu a
 * nastaví na nìj ukazatel result.
 * @param   ukazatel na zásobník
 * @param   ukazatel na adresu výsledku
 * @return  chybový kód
 */
int returnResult(TStack *S, TVar **ptrResult) {

   int err = SYN_ERR;
   TStackData *top = NULL;

   if (!stackEmpty(S)) {
      top = (TStackData *) stackTopPop(S);
      if (top->token == EXPRESSION) {

         // ze stacku se podaøilo vyjmout výraz
         *ptrResult = top->var;
         err = EOK;
      }
      free(top);
   }

   return err;
}

////////////////////////////////////////////////////////////////////////  Vytváøení a mazání pomocných struktur

/*
 * Vytvoøí pomocnou promìnnou a vrátí na ni ukazatel.
 * Promìnné se pro ka¾dý výraz "recyklují" :
 * Pro dal¹í výraz se pou¾ijí tyté¾ promìnné, pokud dojdou,
 * vlo¾í se do seznamu nová promìnná. Øe¹eno pomocí aktivního prvku.
 * @param   ukazatel na seznam pomocných promìnných
 * @param   ukazatel na chybu
 * @return  ukazatel na vytvoøenou promìnnou
 */
TVar *createTmpVar(TList *L, int *err) {

   // posuneme aktivitu na dal¹í prvek seznamu :
   listSucc(L);

   // aktivita se ztratila, je tøeba vytvoøit novou promìnnou
   if (!listActive(L)) {

      // alokace promìnné a dat
      TVar *pom = NULL;
      if ( (pom = malloc(sizeof(TVar))) != NULL) {
         if ( (pom->varData = (TVarData*)malloc(sizeof(TVarData))) != NULL ) {

            pom->varType = VT_TMP_VAR;        // inicializace
            pom->name = NULL;
            pom->varData->type = NIL;
            *err = listInsertLast(L, pom);    // vlo¾ení do seznamu
            listLast(L);                      // stane se aktivním
         } else {
            *err = INTR_ERR;                  // chyba pøi alokaci
            free(pom);
         }
      } else *err = INTR_ERR;
   }

   //  vrátí ukazatel na promìnnou nebo NULL
   if (*err == EOK) {
      return (TVar*)listGetActive(L)->data;
   } else return NULL;
}

/*
 * Vytvoøí a inicializuje data pro zásobník.
 * @param   token
 * @param   ukazatel na promìnnou
 * @param   ukazatel na chybu
 * @return  ukazatel na vytvoøená data
 */
TStackData *createStackData(int token, TVar *var, int *err) {

   TStackData *data = NULL;
   if ( (data = (TStackData*)malloc(sizeof(TStackData))) != NULL ) {
      data->token = token;
      data->var = var;
   } else *err = INTR_ERR; // nedostatek pamìti

   return data;
}

/*
 * Smazání v¹ech dat v zásobníku.
 * @param   ukazatel na zásobník
 */
void stackDataDelete(TStack *S) {

   TSItem *pom = S->top;
   while (pom != NULL) {
      free (pom->data);
      pom = pom->next;
   }
}

////////////////////////////////////////////////////////////////////////  Funkce pro kontrolní výpisy

/*
 * Vytiskne precedenèní tabulku
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
 * Vytiskne obsah zásobníku
 */
void tiskniStack (TStack *s) {

   printf("Stav zásobníku: top\n");
   TSItem *pom = s->top;
   while (pom != NULL) {
      printf("\t%d\n",((TStackData*)(pom->data))->token);
      pom = pom->next;
   }
   printf("_______bottom_________\n\n");
}

/* konec expression.c */
