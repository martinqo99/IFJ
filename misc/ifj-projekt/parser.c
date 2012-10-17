/*
 * @description   Syntakticka analyza shora dolu (rekurzivnim sestupem)
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

/*
 * PRAVIDLA
     1.       <program> -> function <def_func>
     2.      <def_func> -> main ( ) <stat> end ; <EOF>
     3.      <def_func> -> idFunc ( <params> ) <stat> end <program>
     4.        <params> -> eps
     5.        <params> -> id <params_n>
     6.      <params_n> -> eps
     7.      <params_n> -> , id <params_n>
     8.          <stat> -> <def_var> <stat_list>
     9.       <def_var> -> eps
     10.      <def_var> -> local id <INIT> ; <def_var>
     11.         <init> -> eps
     12.         <init> -> = <lit>
     13.          <lit> -> literal
     14.    <stat_list> -> eps
     15.    <stat_list> -> <commad> ; <stat_list>
     16.      <command> -> if expression then <stat_list> else <stat_list> end
     17.      <command> -> while expression do <stat_list> end
     32.      <command> -> repeat <stat_list> until exp
     18.      <command> -> return expression
     19.      <command> -> write ( expression <expression_n> )
     20. <expression_n> -> eps
     21. <expression_n> -> , expression <expression_n>
     22.      <command> -> id = <assign>
     23.       <assign> -> expression
     24.       <assign> -> read ( <lit> )
     25.       <assign> -> idFunc ( <params> )
     26.   <var_params> -> eps
     27.   <var_params> -> <var> <var_n>
     28.          <var> -> <lit>
     29.          <var> -> id
     30.        <var_n> -> eps
     31.        <var_n> -> , <var> <var_n>
*/

#include "parser.h"

#define NEXT_TOKEN token = getNextToken(&attr); if(token < 0) return token;

int prsProgram();
int prsDefFunc();
int prsStat();
int prsParams();
int prsParamsN();
int prsDefVar();
int prsStatList();
int prsInit(string*);
int prsLit();
int prsCommand();
int prsExpN();
int prsAssign(TVar*); // promenou predavam kvuli generovani instrukci
int prsVarParams();
int prsVar();
int prsVarN();

int parser(TTable *t) {
   table = t;
   strInit(&attr);

   int err = prsProgram();
   strFree(&attr);
   return err;
}

int prsProgram() {
   // 1. <program> -> function <def_func>
   NEXT_TOKEN
   if(token != KW_FUNCTION) return SYN_ERR;

   return prsDefFunc();
}

int prsDefFunc() {
   int err;
   TInstr *i;

   NEXT_TOKEN
   switch(token) {
   case KW_MAIN: {
      // 2. <def_func> -> main ( ) <stat> end ; <EOF>
      if( (err = tableInsertFunction(table, attr) ) != INS_OK) {
         switch(err) {
         case INS_NODE_EXIST:
            return SEM_ERR;   // tato fce uz v tabulce je (NEMELA BY BYT!!!)
         case INS_MALLOC:
         default:
            return INTR_ERR;
         }
      }

      instr = &(table->lastAddedFunc->instructions);
      NEXT_TOKEN
      if(token != L_LEFT_BRACKET) return SYN_ERR;

      NEXT_TOKEN
      if(token != L_RIGHT_BRACKET) return SYN_ERR;

      // vlozim instrukci na vyprazdeni zasobniku protoze main muze byt volany rekuzivne
      // a nejaky sikula by mainu mohl dat parametry
      if( (i = genInstr(I_STACK_E, NULL, NULL, NULL) ) == NULL )
         return INTR_ERR;
      if( listInsertLast( instr,  i) != LIST_EOK)
         return INTR_ERR;

      err = prsStat();
      if(err != PRS_OK) return err;

      // attr uz nacetl prsStat
      if(token != KW_END) return SYN_ERR;

      NEXT_TOKEN
      if(token != L_SEMICOLON) return SYN_ERR;

      NEXT_TOKEN;
      if(token != END_OF_FILE) return SYN_ERR;

      return PRS_OK;
   }
   break;
   case L_ID: {
      // 3. <def_func> -> idFunc ( <params> ) <stat> end <program>
      if( (err = tableInsertFunction(table, attr) ) != INS_OK) {
         switch(err) {
         case INS_NODE_EXIST:
            return SEM_ERR;
         case INS_MALLOC:
         default:
            return INTR_ERR;
         }
      }

      // povedlo vlozit
      instr = &(table->lastAddedFunc->instructions);

      NEXT_TOKEN
      if(token != L_LEFT_BRACKET) return SYN_ERR;

      err = prsParams();
      if(err != PRS_OK) return err;

      // vyprazdnim zasobnik kdyby mi nahodou nekdo predal funkci vic parametru nez ocekava
      if( (i = genInstr(I_STACK_E, NULL, NULL, NULL)) == NULL)
         return INTR_ERR;
      if( listInsertLast( instr, i ) != LIST_EOK)
         return INTR_ERR;

      // dalsi attr uz nacetl prsParams
      if(token != L_RIGHT_BRACKET) return SYN_ERR;

      err = prsStat();
      if(err != PRS_OK) return err;

      // dalsi attr uz nacetl prsStat
      if(token != KW_END) return SYN_ERR;

      return prsProgram();
   }
   break;
   case KW_SORT:
   case KW_FIND:
   case KW_SUBSTR:
   case KW_TYPE:
      return SEM_ERR;
      break;
   }
   return SYN_ERR;
}

int prsParams() {
   int err;
   TInstr *i;
   NEXT_TOKEN
   // 4. <params> -> eps
   if(token == L_RIGHT_BRACKET) return PRS_OK;

   // 5. <params> -> id <params_n>
   if(token != L_ID && token != KW_MAIN) return SYN_ERR;

   // jestli se id jmenuje stejne jako nejaka funkce SEM_ERR
   if(tableSearchFunction(table, attr) != NULL) return SEM_ERR;
   // pokusim se id vlozit to tabulky
   err = functionInsertVar(table->lastAddedFunc, attr);
   if(err != INS_OK) {
      switch(err) {
      case INS_NODE_EXIST:
         return SEM_ERR;
      case INS_MALLOC:
      default:
         return INTR_ERR;
      }
   }
   // nageneruju instrukci
   if( (i = genInstr(I_SET, getLastAddedVar(table->lastAddedFunc), NULL, NULL) ) == NULL)
      return INTR_ERR;
   // vlozim
   if( listInsertLast( instr, i ) != LIST_EOK)
      return INTR_ERR;

   if( (i = genInstr(I_POP, getLastAddedVar(table->lastAddedFunc), NULL, NULL) ) == NULL)
      return INTR_ERR;
   if( listInsertLast( instr, i ) != LIST_EOK)
      return INTR_ERR;

   err = prsParamsN();
   if(err != PRS_OK) return err;

   return PRS_OK;
}

int prsParamsN() {
   int err;
   TInstr *i;

   NEXT_TOKEN
   // 6. <params_n> -> eps
   if(token == L_RIGHT_BRACKET) return PRS_OK;

   // 7. <params_n> -> , id <params_n>
   if(token != L_COMMA) return SYN_ERR;

   NEXT_TOKEN
   if(token != L_ID && token != KW_MAIN) return SYN_ERR;

   // jestli se id jmenuje stejne jako nejaka funkce SEM_ERR
   if(tableSearchFunction(table, attr) != NULL) return SEM_ERR;
   // pokusim se id vlozit to tabulky
   err = functionInsertVar(table->lastAddedFunc, attr);
   if(err != INS_OK) {
      switch(err) {
      case INS_NODE_EXIST:
         return SEM_ERR;
      case INS_MALLOC:
      default:
         return INTR_ERR;
      }
   }

   // nageneruju instrukci
   if( (i = genInstr(I_SET, getLastAddedVar(table->lastAddedFunc), NULL, NULL) ) == NULL)
      return INTR_ERR;
   // vlozim
   if( listInsertLast( instr, i ) != LIST_EOK)
      return INTR_ERR;

   if( (i = genInstr(I_POP, getLastAddedVar(table->lastAddedFunc), NULL, NULL) ) == NULL)
      return INTR_ERR;
   if( listInsertLast( instr, i ) != LIST_EOK)
      return INTR_ERR;

   return prsParamsN();
}

int prsStat() {
   // 8. <stat> -> <def_var> <stat_list>
   int err;
   err = prsDefVar();
   if(err != PRS_OK) return err;

   err = prsStatList();
   if(err != PRS_OK) return err;

   return PRS_OK;
}

int prsDefVar() {
   int err;

   NEXT_TOKEN
   // 9. <def_var> -> eps
   if(token == KW_END   || token == L_ID      || token == KW_IF    || // main muze byt i promena ;)
      token == KW_WHILE || token == KW_RETURN || token == KW_WRITE ||
      token == KW_MAIN  || token == KW_REPEAT)
      return PRS_OK;

   // 10. <def_var> -> local id <INIT> ; <def_var>
   if(token != KW_LOCAL) return SYN_ERR;

   NEXT_TOKEN
   if(token == KW_TYPE || token == KW_SUBSTR || token == KW_FIND || token == KW_SORT)
      return SEM_ERR;
                       // promena v uzivatelem def fci se muze jmenovat main
   if(token != L_ID && token != KW_MAIN) return SYN_ERR;
   // prohledam jestli se nejaka promena nejmenuje stejne jako nejaka funkce
   if(tableSearchFunction(table, attr) != NULL) return SEM_ERR;

   string tmp = strCreateString(&attr);
   err = prsInit(&tmp);
   strFree(&tmp);
   if(err != PRS_OK) return err;

   if(token != L_SEMICOLON) return SYN_ERR;

   return prsDefVar();
}

int prsInit(string *tmp) {
   int err;
   TInstr *i;

   NEXT_TOKEN
   // 11. <init> -> eps
   if(token == L_SEMICOLON) {
      // vlozim promenou doi tabulky
      err = functionInsertVar(table->lastAddedFunc, *tmp);
      if(err != INS_OK) {
         switch(err) {
            case INS_NODE_EXIST:return SEM_ERR;
            case INS_MALLOC:
            default: return INTR_ERR;
         }
      }
      // inicializuji promenou na nil
      if( (i = genInstr(I_SET, getLastAddedVar(table->lastAddedFunc), NULL, NULL) )  == NULL) return INTR_ERR;
      if( listInsertLast( instr, i ) != LIST_EOK) return INTR_ERR;
      return PRS_OK;
   }
   // 12. <init> -> = expression
   if(token != L_ASSIGN)  return SYN_ERR;

   NEXT_TOKEN
   // naparsuju vyraz
   TVar *tmpV;
   if( (err = parseExpression(table, &tmpV)) != EOK) return err;

   // pokud parsovani vyrazu probehlo v poradku tak az teprve ted vlozim promenou
   // do tabulky. nesmim zapomenout ze parsovani mi nacestlo dalso token!!! nejspis ;
   err = functionInsertVar(table->lastAddedFunc, *tmp);
   if(err != INS_OK) {
      switch(err) {
         case INS_NODE_EXIST:return SEM_ERR;
         case INS_MALLOC:
         default: return INTR_ERR;
      }
   }
   // nageneruji instrukci inicializace promene
   if((i = genInstr(I_SET, getLastAddedVar(table->lastAddedFunc), tmpV, NULL))  == NULL)
      return INTR_ERR;

   if(listInsertLast(instr, i) != EOK) return INTR_ERR;

   return PRS_OK;
}

int prsLit() {
   // 13. <lit> -> literal // nejaky z literalu
   if( token == KW_NIL || token == KW_FALSE || token == KW_TRUE || token == L_NUMBER || token == L_STRING )
      return PRS_OK;
   return SYN_ERR;
}

int prsStatList() {
   // TOKEN UZ JE NACTENY
   int err;
   // 14. <stat_list> -> eps
   if(token == KW_END || token == KW_ELSE || token == KW_UNTIL) return PRS_OK;

   // 15. <stat_list> -> <commad> ; <stat_list>
   // if( token != L_ID && token != KW_MAIN   && token != KW_IF   && token != KW_WHILE   && token != KW_RETURN  && token != KW_WRITE )
   //   return SYN_ERR;

   err = prsCommand();
   if(err != PRS_OK) return err;

   // attr uz je nacteny
   if(token != L_SEMICOLON) return SYN_ERR;

   NEXT_TOKEN // <stat_list> ceka uz nacteny attr

   return prsStatList();
}

int prsCommand() {
   // TOKEN UZ JE NACTENY
   TVar *tmpV;
   int err = INTR_ERR;
   switch(token) {
      // 22. <command> -> id = <assign>
   case KW_MAIN:
   case L_ID:
      // je id v tabulce symbolu pro tuhle funkci?
      if( (tmpV = functionSearchVar(table->lastAddedFunc, attr) ) == NULL ) return SEM_ERR;
      NEXT_TOKEN
      if(token != L_ASSIGN) return SYN_ERR;

      err = prsAssign(tmpV);
      if(err != PRS_OK) return err;

      return PRS_OK;
   break;
   // 16. <command> -> if expression then <stat_list> else <stat_list> end
   case KW_IF: {
      NEXT_TOKEN
      // naparsuju vyraz
      if( (err = parseExpression(table, &tmpV)) != EOK) return err;
      // ocekavam then
      if(token != KW_THEN) return SYN_ERR;
      // pomocna navesti
      TInstr *labElse  = genInstr(I_LAB, NULL, NULL, NULL);
      TInstr *labEndIf = genInstr(I_LAB, NULL, NULL, NULL);
      // potrebne instrukce, navesti doplnim pozdeji
      TInstr *jmpz = genInstr(I_JMP_Z, NULL, tmpV, NULL);
      TInstr *jmp  = genInstr(I_JMP, NULL , NULL, NULL);

      TLItem *itmElse = NULL;
      TLItem *itmEnd  = NULL;

      if(labElse == NULL || labEndIf == NULL || jmpz == NULL || jmp == NULL )
         return INTR_ERR;

      // JMP_Z tmp labElse
      if( listInsertLast(instr, jmpz) != LIST_EOK)
         return INTR_ERR;

      //nactu dalsi token
      token = getNextToken(&attr);
      if(token < 0) {
         free(labElse);
         free(labEndIf);
         free(jmp);
         return token;
      }

      if( ( err = prsStatList() ) != PRS_OK || token !=  KW_ELSE) {
         free(labElse);
         free(labEndIf);
         free(jmp);
         return err == PRS_OK ? SYN_ERR : err;
      }

      // JMP labEndIf
      if( listInsertLast(instr, jmp) != LIST_EOK)
         return INTR_ERR;

      // LAB labElse
      if( listInsertLast(instr, labElse) != LIST_EOK)
         return INTR_ERR;
      itmElse = instr->Last;

      //nactu dalsi token
      token = getNextToken(&attr);
      if(token < 0) {
         free(labEndIf);
         return token;
      }

      if( ( err = prsStatList() ) != PRS_OK || token != KW_END) {
         free(labEndIf);
         return err == PRS_OK ? SYN_ERR : err;
      }

      // LAB labEndIf
      if( listInsertLast(instr, labEndIf) != LIST_EOK)
         return INTR_ERR;
      itmEnd = instr->Last;

      jmp->dest  = itmEnd;
      jmpz->dest = itmElse;

      NEXT_TOKEN
      return PRS_OK;
   }
   break;
   // 17. <command> -> while expression do <stat_list> end
   case KW_WHILE: {
      TInstr *labWhile = genInstr(I_LAB, NULL, NULL, NULL);
      TInstr *labEnd   = genInstr(I_LAB, NULL, NULL, NULL);

      TLItem *jmpToWhile = NULL;
      TLItem *jmpToEnd   = NULL;

      if(labWhile == NULL || labEnd == NULL)
         return INTR_ERR;
      // navesti zacatku cyklu
      if(listInsertLast(instr, labWhile) != LIST_EOK)
         return INTR_ERR;

      // ulozim si adresu vygenerovaneho navesti
      jmpToWhile = instr->Last;

      // nactu dalsi token
      token = getNextToken(&attr);
      if(token < 0) {
         free(labEnd);
         return token;
      }
      // zpracuju vzraz
      if( (err = parseExpression(table, &tmpV)) != EOK || token != KW_DO) {
         free(labEnd);
         return err == PRS_OK ? SYN_ERR : err;
      }

      // porovani pripadny skok na konec cyklu
      // misto kam se bude skakat se doplni pozdeji
      TInstr *jmpz = genInstr(I_JMP_Z, NULL, tmpV, NULL);

      if(jmpz == NULL) return INTR_ERR;
      if(listInsertLast(instr, jmpz) != LIST_EOK) return INTR_ERR;

      //nactu dalsi token
      token = getNextToken(&attr);
      if(token < 0) {
         free(labEnd);
         return token;
      }
      // naparsuju vnitrek cyklu
      int err = prsStatList();
      if(err != PRS_OK || token != KW_END) {
         free(labEnd);
         return err == PRS_OK ? SYN_ERR : err;
      }

      TInstr *jmp = genInstr(I_JMP, jmpToWhile, NULL, NULL);
      if(jmp == NULL) return INTR_ERR;
      if(listInsertLast(instr, jmp) != LIST_EOK)
         return INTR_ERR;

      // navesti konce cyklu
      if(listInsertLast(instr, labEnd) != LIST_EOK)
         return INTR_ERR;
      jmpToEnd = instr->Last;

      jmpz->dest = jmpToEnd;

      NEXT_TOKEN
      return PRS_OK;
   }
   break;
   // 32. <command> -> repeat <stat_list> until exp
   case KW_REPEAT:{
      TInstr *tmp = genInstr(I_LAB, NULL, NULL, NULL);
      if(tmp == NULL) return INTR_ERR;

      if(listInsertLast(instr, tmp) != LIST_EOK) return INTR_ERR;
      TLItem *lab = instr->Last;

      NEXT_TOKEN
      if( (err = prsStatList()) != PRS_OK || token != KW_UNTIL) return err == PRS_OK ? SYN_ERR : err;

      NEXT_TOKEN
      if( (err = parseExpression(table, &tmpV)) != EOK || token != L_SEMICOLON) return err == PRS_OK ? SYN_ERR : err;

      tmp = genInstr(I_JMP_Z, lab, tmpV, NULL);
      if(tmp == NULL) return INTR_ERR;

      if(listInsertLast(instr, tmp) != LIST_EOK) return INTR_ERR;

      return PRS_OK;
   }break;
   // 18. <command> -> return expression
   case KW_RETURN: {
      // preskocim vyraz a vratim ze bylo vse OK
      NEXT_TOKEN
      if((err = parseExpression(table, &tmpV)) != EOK)
         return err;

      TInstr *push = genInstr(I_PUSH, tmpV, NULL, NULL);
      TInstr *ret = genInstr(I_RETURN, NULL, NULL, NULL);
      if(push == NULL || ret == NULL)   return INTR_ERR;

      if(listInsertLast(instr, push) != EOK) return INTR_ERR;
      if(listInsertLast(instr, ret) != EOK) return INTR_ERR;

      return PRS_OK;
   }
   break;
   // 19. <command> -> write ( expression <expression_n> )
   case KW_WRITE: {
      NEXT_TOKEN
      if(token != L_LEFT_BRACKET) return SYN_ERR;

      NEXT_TOKEN
      if(token == KW_TRUE || token == KW_FALSE || token == KW_NIL) return SEM_ERR;
      // vypocitam vyraz
      if((err = parseExpression(table, &tmpV)) != EOK) return err;
      // nageneruju instrukci
      TInstr *wrt = genInstr(I_WRITE, tmpV, NULL, NULL);
      if(wrt == NULL) return INTR_ERR;
      if(listInsertLast(instr, wrt) != LIST_EOK) return INTR_ERR;

      err = prsExpN();
      if(err != PRS_OK ) return err;

      if(token != L_RIGHT_BRACKET) return SYN_ERR;

      NEXT_TOKEN
      return PRS_OK;
   }
   break;
   }
   return SYN_ERR;
}

int prsExpN() {
   int err = INTR_ERR;
   TVar *tmpV;
   // 20. <expression_n> -> eps
   if(token == L_RIGHT_BRACKET) return PRS_OK;

   // 21. <expression_n> -> , expression <expression_n>
   if(token != L_COMMA) return SYN_ERR;
   NEXT_TOKEN
   if((err = parseExpression(table, &tmpV)) != EOK) return err;

   TInstr *wrt = genInstr(I_WRITE, tmpV, NULL, NULL);
   if(wrt == NULL) return INTR_ERR;
   if(listInsertLast(instr, wrt) != LIST_EOK) return INTR_ERR;

   return prsExpN();
}

int prsAssign(TVar *var) {

   NEXT_TOKEN
   if(token == KW_READ) {
      // 24. <assign> -> read ( <lit> )
      NEXT_TOKEN
      if(token != L_LEFT_BRACKET) return SYN_ERR;

      NEXT_TOKEN
      if(token != L_STRING && token != L_NUMBER ) return SYN_ERR;

      // vlozim konstantu
      if( functionInsertConstatnt(table->lastAddedFunc, attr,  token) != INS_OK )
         return INTR_ERR;
      // vzgeneruju instrukci
      TInstr *read = genInstr(I_READ, var, table->lastAddedFunc->constants.Last->data, NULL);
      if( listInsertLast( instr,  read) != LIST_EOK)
         return INTR_ERR;

      NEXT_TOKEN
      if(token != L_RIGHT_BRACKET) return SYN_ERR;

      NEXT_TOKEN
      return PRS_OK;
   }

   int tokenTmp = token; // ulozim si attr abych potom vedel jakou instrukci generovat
   int err;
   TFunction *Ftmp;

   Ftmp = tableSearchFunction(table, attr);

   if(Ftmp == NULL && (token != KW_TYPE && token != KW_SUBSTR && token != KW_FIND && token != KW_SORT) ) {
      // 23. <assign> -> expression
      TVar *tmpV;
      if( (err = parseExpression(table, &tmpV)) != EOK) return err;

      TInstr *mov = genInstr(I_MOV, var, tmpV, NULL);
      if(mov == NULL) return INTR_ERR;

      if(listInsertLast(instr, mov) != EOK) return INTR_ERR;

      return PRS_OK;
   }

   // 25. <assign> -> idFunc ( <params> )
   NEXT_TOKEN
   if(token != L_LEFT_BRACKET) return SYN_ERR;

   err = prsVarParams();
   if(err != PRS_OK) return err;

   if(token != L_RIGHT_BRACKET) return SYN_ERR;

   TInstr *tmpInstr = NULL;
   switch(tokenTmp) {
   case KW_MAIN:
   case L_ID: {
      tmpInstr = genInstr(I_CALL, Ftmp, NULL, NULL);
      if(tmpInstr == NULL) return INTR_ERR;

      if( listInsertLast( instr,  tmpInstr) != LIST_EOK)
         return INTR_ERR;

      tmpInstr = genInstr(I_POP, var, NULL, NULL);
      if(tmpInstr == NULL) return INTR_ERR;

      if( listInsertLast( instr,  tmpInstr) != LIST_EOK)
         return INTR_ERR;
      NEXT_TOKEN
      return PRS_OK;
   }
   break;
   case KW_TYPE: {
      tmpInstr = genInstr(I_TYPE, var, NULL, NULL);
   }
   break;
   case KW_SUBSTR: {
      tmpInstr = genInstr(I_SUBSTR, var, NULL, NULL);
   }
   break;
   case KW_FIND: {
      tmpInstr = genInstr(I_FIND, var, NULL, NULL);
   }
   break;
   case KW_SORT: {
      tmpInstr = genInstr(I_SORT, var, NULL, NULL);
   }
   break;
   }

   if(tmpInstr == NULL) return INTR_ERR;

   if( listInsertLast( instr,  tmpInstr) != LIST_EOK)
      return INTR_ERR;

   NEXT_TOKEN
   return PRS_OK;
}

int prsVarParams() {
   NEXT_TOKEN
   // 26. <var_params> -> eps
   if(token == L_RIGHT_BRACKET) return PRS_OK;

   // 27. <var_params> -> <var> <var_n>
   // parametry musim dat v opacnem poradi
   // nastavim aktivitu na posledni prvek seznamu instrukci
   listLast(instr);
   // vsechny dalsi instrukce dava za aktivni prvek!!!

   int err = prsVar();
   if(err != PRS_OK) return err;

   err = prsVarN();
   if(err != PRS_OK) return err;

   if(token == L_RIGHT_BRACKET) return PRS_OK;

   return SYN_ERR;
}

int prsVar() {
   // 28. <var> -> <lit>
   TInstr *tmpInstr;
   int err = prsLit();
   if(err == PRS_OK) {
      // konstantka
      // vztvorim konstantu
      if(functionInsertConstatnt(table->lastAddedFunc, attr, token ) != INS_OK )
         return INTR_ERR;
      // vytvorim instrukci
      TVar *con = table->lastAddedFunc->constants.Last->data;

      if( (tmpInstr = genInstr(I_PUSH, con, NULL, NULL)) == NULL) return INTR_ERR;

      if( listPostInsert ( instr, tmpInstr) != LIST_EOK)
         return INTR_ERR;

   }
   if(err == SYN_ERR) {
      // 29. <var> -> id
      if(token != L_ID && token != KW_MAIN) return SYN_ERR;
      TVar *tmp = functionSearchVar(table->lastAddedFunc, attr);
      if(tmp == NULL)   return SEM_ERR;

      if( (tmpInstr = genInstr(I_PUSH, tmp, NULL, NULL)) == NULL) return INTR_ERR;

      if( listPostInsert( instr, tmpInstr ) != LIST_EOK)
         return INTR_ERR;

      return PRS_OK;
   }
   return err;
}

int prsVarN() {
   NEXT_TOKEN
   // 30. <var_n> -> eps
   if(token == L_RIGHT_BRACKET) return PRS_OK;
   // 31. <var_n> -> , <var> <var_n>
   if(token != L_COMMA) return SYN_ERR;

   NEXT_TOKEN
   int err = prsVar();
   if(err != PRS_OK) return err;

   err = prsVarN();
   if(err != PRS_OK) return err;

   return PRS_OK;
}
