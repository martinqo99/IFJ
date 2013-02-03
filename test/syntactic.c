/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    syntactic.c
 * Popis:     Syntakticka analyza
 * Datum:     17.11.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "syntactic.h"

/**
 * <program> - <body_program>
 * <body_program> - <def_function><body_program>
 * <body_program> - <command><body_program>
 * <body_program> - eps
 * <def_function> - function idFunction (<params>) EOL <stat_list> end EOL
 * <stat_list> - eps
 * <stat_list> - <command> <stat_list>
 * <params> - id <params_n>
 * <params> - eps
 * <params_n> - , id <params_n>
 * <params_n> - eps
 * <command> - id = <assign>
 * <command> - if expression EOL <stat_list> else EOL <stat_list> end EOL
 * <command> - while expression EOL <stat_list> end EOL
 * <command> - return expression EOL
 * <assign> - expression
 * <assign> - idFunction( <params> )
 * <assign> - input()
 * <assign> - numeric(id)
 * <assign> - print( <term> )
 * <assign> - typeof(id)
 * <assign> - len(id)
 * <assign> - find(string, string) || find (id, id)
 * <assign> - sort(string) || sort (id)
 * <assign> - string[ <num>:<num> ] EOL
 * <num> - eps
 * <num> - num
 * <term> - id
 * <term> - <value>
 * <value> - num
 * <value> - string
 * <value> - logic
**/

/**
 * pravidla OLD VERSION!!!
 * <program>      -> <program_body> end EOF
 * <program_body> -> eps
 * <program_body> -> <def_function> <program body>
 * <program_body> -> <stat_list> <program_body>
 * <stat_list>    -> eps
 * <stat_list>    -> <command> <stat_list>
 * <def_function> -> function idFunc ( <params> ) EOL <stat_list> end
 * <params>       -> eps
 * <params>       -> id <params_n>
 * <params_n>     -> eps
 * <params_n>     -> ,<params_n>
 * <command>      -> if expr EOL <stat_list> <else> end EOL
 * <else>         -> else EOL <stat_list>
 * <else>         -> eps
 * <command>      -> while expr EOL <stat_list> end EOL
 * <command>      -> return expr EOL
 * <command>      -> id = stringId [<num>:<num>]EOL
 * <num>          -> eps
 * <num>          -> num
 * <command>      -> id = idFunc ( <params> ) EOL
 */

/**
 * @info      Hlavni fce syntakticke analyzy
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE parser (tSymbolTable *table)
{
  //tady bude jeste prvni prubeh pro pridani ID funkci do tabulky
    E_CODE err;
    if((err=findDefFunctions(table))!=ERROR_OK)return err;
    //printf("prvni pruchod ukoncen");
    return prsBody(table);

}

/**
 * @info      Vyhledání všech definic funkcí
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE findDefFunctions(tSymbolTable *table)
{
    E_CODE err=ERROR_OK;
    tKeyword kw;
    while((kw=getToken())!=LEX_EOF){
        if(kw==LEX_UNKNOWN || kw==LEX_ERROR)
            return ERROR_LEX;
        else if (kw==LEX_RESERVED)
            return ERROR_SYNTAX;
        else if(kw==KW_FUNCTION){
            if(getToken()==LEX_ID){
                err=symbolTableInsertFunction(table,gToken.data);
                if (err==ERROR_INS_EXIST) return ERROR_SEMANTIC; //dvojita definice fce
            }
            else return ERROR_SYNTAX;
        }
    }
    resetToken();
    initToken();
    return err;
}

/**
 * @info      Analyza tela programu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsBody (tSymbolTable *table)
{
    E_CODE err = ERROR_OK;
    tKeyword kw;
    table->currentFunc=&(table->mainFunc);
    while ((kw = getToken()) == LEX_EOL);//procykli prazdne radky
    switch (kw) {
        case LEX_EOF: return err;
        case KW_FUNCTION:{
            if((err = prsDefFunction(table)) != ERROR_OK) return err;
            else return prsBody(table);
            }
       default:{
            if((err = prsCommand(table,kw)) != ERROR_OK) return err;
            else return prsBody(table);
            }
    }
}

/**
 * @info      Analyza prikazu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsCommand (tSymbolTable *table, tKeyword kw)
{
    E_CODE err = ERROR_OK;
    tInstr *i,*jmp1,*jmp2;
    tSymbol *expResult;
    switch (kw){
        case LEX_ID:{
        //tady je treba pridat ID do tabulky, jestli tam uz neni
        //checkni =, zavolej prsAssign
            if(functionInsertSymbol(table->currentFunc,gToken.data)!=ERROR_INS_EXIST){
                //toto je prvni definice promenne
                if(symbolTableSearchFunction(table,gToken.data)!=NULL)
                    return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
            }
            if (getToken() != LEX_ASSIGN) return ERROR_SYNTAX;
            else if ((err = prsAssign(table,getLastSymbol(table->currentFunc))) != ERROR_OK) return err;
        }
        break;
        case KW_IF:{
        //checkni if/else vetev: if expression EOL <stat_list> else EOL <stat_list> end EOL
            kw=getToken();
            if ((err = prsExpression(table, kw,&expResult)) != ERROR_OK) return err;
            jmp1=genInstr(I_FJUMP,NULL,expResult,NULL); //jump to else
            listInsertLast(&(table->currentFunc->instructions),jmp1);
            //statlist IF vetve
            if ((err = prsStatlist(table)) != ERROR_ELSE) return err; //posledni token je else
            if (getToken() != LEX_EOL) return ERROR_SYNTAX; 
            jmp2=genInstr(I_JUMP,NULL,NULL,NULL); //jump to end of else
            listInsertLast(&(table->currentFunc->instructions),jmp2);
            i=genInstr(I_LABEL,NULL,NULL,NULL); //navesti zacatku else
            listInsertLast(&(table->currentFunc->instructions),i);
            jmp1->dest=table->currentFunc->instructions.last;
            //statlist else vetve
            if ((err=prsStatlist(table)) != ERROR_OK) return err; //end se nacte v prsStatlist
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_LABEL,NULL,NULL,NULL); //navesti konce else
            listInsertLast(&(table->currentFunc->instructions),i);
            jmp2->dest=table->currentFunc->instructions.last;
        }
        break;
        case KW_WHILE:{
        //while loop: while expression EOL <stat_list> end EOL
            kw=getToken();
            jmp2=genInstr(I_JUMP,NULL,NULL,NULL); //jmp to start while -skokova instrukce je vygenerovana dopredu

            i=genInstr(I_LABEL,NULL,NULL,NULL); //navesti zacatku while
            listInsertLast(&(table->currentFunc->instructions),i);
            jmp2->dest=table->currentFunc->instructions.last;
            if ((err = prsExpression(table, kw,&expResult)) != ERROR_OK) return err;
            jmp1=genInstr(I_FJUMP,NULL,expResult,NULL); //jump to end of while
            listInsertLast(&(table->currentFunc->instructions),jmp1);
            if ((err = prsStatlist(table)) != ERROR_OK) return err;//end se nacte v prsStatlist
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            listInsertLast(&(table->currentFunc->instructions),jmp2);//jump to start of while
            i=genInstr(I_LABEL,NULL,NULL,NULL); //navesti konce while
            listInsertLast(&(table->currentFunc->instructions),i);
            jmp1->dest=table->currentFunc->instructions.last;
        }
        break;
        case KW_RETURN:{
        //return function: return expression EOL
            kw=getToken();
            if ((err = prsExpression(table, kw,&expResult)) != ERROR_OK) return err;
            i=genInstr(I_PUSH,expResult,NULL,NULL); //push vysledek expr
            listInsertLast(&(table->currentFunc->instructions),i);
            i=genInstr(I_RETURN,NULL,NULL,NULL); //instrukce return
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        default: return ERROR_SYNTAX;
    }
    return err;
}

/**
 * @info      Analyza definice funkce
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsDefFunction (tSymbolTable *table)
{
//function idFunction (<params>) EOL <stat_list> end EOL
    E_CODE err = ERROR_OK;
    if (getToken() != LEX_ID) return ERROR_SYNTAX;
    table->currentFunc=symbolTableSearchFunction(table,gToken.data);
    if (table->currentFunc==NULL)return ERROR_COMPILATOR;
    if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
    if ((err = prsDefParams(table)) != ERROR_OK) return err;//prava zavorka se nacte uvnitr params
    tInstr *i=genInstr(I_SEMPTY,NULL,NULL,NULL);//po precteni parametru vyprazdnit stack
    listInsertLast(&(table->currentFunc->instructions),i);
    if (getToken() != LEX_EOL) return ERROR_SYNTAX;
    if ((err = prsStatlist(table)) != ERROR_OK) return err;//posledni nacteny kw je end
    if (getToken() != LEX_EOL) return ERROR_SYNTAX;
    return err;
}

/**
 * @info      Analyza kodu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsStatlist (tSymbolTable *table)
{
  // <stat_list> - eps
  // <stat_list> - <command> <stat_list>
  //posledni nacteny token byl EOL
    E_CODE err = ERROR_OK;
    tKeyword kw;
    while ((kw = getToken()) == LEX_EOL); //procykli prazdne radky
    if (kw == KW_END) return ERROR_OK;
    if (kw == KW_ELSE) return ERROR_ELSE;
    if ((err = prsCommand(table,kw)) == ERROR_OK) return prsStatlist(table); //@kw - prsCommand potrebuje znat posledni token
    else return err;
}

/**
 * @info      Analyza prirazovacich prikazu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @param   tSymbol* - ukazatel kam se prirazuje
 * @return  E_CODE - chybovy kod
 */
E_CODE prsAssign (tSymbolTable *table,tSymbol *dest)
{
  // <assign> - expression
  // <assign> - idFunction( <params> )
  // <assign> - input()
  // <assign> - numeric(id)
  // <assign> - typeof(id)
  // <assign> - len(id)
  // <assign> - print( <term> )
  // <assign> - find(string, string) || find (id, id)
  // <assign> - sort(string) || sort (id)
  // <assign> - string[ <num>:<num> ]
    E_CODE err = ERROR_OK;
    tKeyword kw;
    tSymbol *tmpSymb, *expResult;
    tFunction *tmpFunc;
    tInstr *i;
    switch (kw = getToken()){
        case KW_INPUT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_INPUT,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        case KW_NUMERIC:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_NUMERIC,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        case KW_LEN:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_LEN,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        case KW_PRINT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_PRINT,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);

        }
        break;
        case KW_FIND:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_FIND,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);

        }
        break;
        case KW_SORT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_SORT,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        case KW_TYPEOF:{
            //typeof ma jine parametry pac bere i funkce
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if((err=prsCallParams(table))!=ERROR_OK) return err;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            i=genInstr(I_TYPEOF,dest,NULL,NULL);
            listInsertLast(&(table->currentFunc->instructions),i);
        }
        break;
        case LEX_ID:{
        //ID -> expression (napr. x+5)
        //ID-> funkce (napr. funkce(x))
        //ID-> string (string[1.0:6.4])
            switch(getTokenAhead()){
                case LEX_L_BRACKET:{//funkce
                    if((tmpFunc=symbolTableSearchFunction(table,gToken.data))==NULL)
                        return ERROR_SEMANTIC_FUNCTION;//nedefinovana fce
                    if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
                    if ((err = prsCallParams(table)) != ERROR_OK) return err; //prava zavorka se checkne uz v prsParams
                    if (getToken() != LEX_EOL) return ERROR_SYNTAX;
                    i=genInstr(I_CALL,tmpFunc,NULL,NULL);//instrukce zavolani fce
                    listInsertLast(&(table->currentFunc->instructions),i);
                    i=genInstr(I_POP,dest,NULL,NULL);//instrukce popnuti navratove hodnoty
                    listInsertLast(&(table->currentFunc->instructions),i);
                }
                break;
                case LEX_L_SBRACKET:{//fce string

                    if((tmpSymb=functionSearchSymbol(table->currentFunc,gToken.data))==NULL)
                        return ERROR_SEMANTIC_VARIABLE;
                    //instrukce co hodi string na stack
                    i=genInstr(I_PUSH,tmpSymb,NULL,NULL);
                    listInsertLast(&(table->currentFunc->instructions),i);
                    err=prsStringselect(table,dest);
                    if (getToken() != LEX_EOL) return ERROR_SYNTAX;
                }
                break;
                default:{//expression
                    err=prsExpression(table, kw, &expResult);
                    i=genInstr(I_MOV,dest,expResult,NULL);//presun vyhodnoceni vyrazu do dest
                    listInsertLast(&(table->currentFunc->instructions),i);
                }
            }
        }
        break;
        case LEX_STRING:{
        //STRING -> string[ : ]
        //STRING -> expression

            if(getTokenAhead()==LEX_L_SBRACKET){
                //hodim string do tabulky konstant
                tmpSymb=functionInsertConstant(table->currentFunc,gToken.data,kw);
                if (err!=ERROR_OK) return err;
                //a vygeneruji instrukci ktera ho da na stack
                i=genInstr(I_PUSH,tmpSymb,NULL,NULL);
                listInsertLast(&(table->currentFunc->instructions),i);
                //naparsuji parametry
                err=prsStringselect(table,dest);
                if (err!=ERROR_OK) return err;
                if (getToken() != LEX_EOL) return ERROR_SYNTAX;
                return err;
            }
        }
        default:{
            err=prsExpression(table, kw, &expResult);
            i=genInstr(I_MOV,dest,expResult,NULL);//presun vyhodnoceni vyrazu do dest
            listInsertLast(&(table->currentFunc->instructions),i);
        }
    }
    return err;
}

E_CODE prsStringselect(tSymbolTable *table, tSymbol *dest)
{
    tSymbol *param1=NULL;
    tSymbol *param2=NULL;
    E_CODE err=ERROR_OK;
    tKeyword kw;
  //err = functionInsertSymbol(table->currentFunc, gToken.data);
  //if (err != ERROR_OK) return err;

    if (getToken() != LEX_L_SBRACKET) return ERROR_SYNTAX;
    if ((kw=getToken()) != LEX_COLON){//prvni param je zadan
        if (kw==LEX_ID){
            param1=functionSearchSymbol(table->currentFunc,gToken.data);
            if (param1==NULL) return ERROR_SEMANTIC_VARIABLE;//nedefinovana promenna
        }
        else if(kw==LEX_NUMBER){
            param1=functionInsertConstant(table->currentFunc,gToken.data,kw);
        }
        else return ERROR_SYNTAX;

        if ((kw=getToken()) != LEX_COLON)return ERROR_SYNTAX;
    }

    //parsujeme druhy param
    if ((kw=getToken()) != LEX_R_SBRACKET){//druhy param je zadan
        if (kw==LEX_ID){
            param2=functionSearchSymbol(table->currentFunc,gToken.data);
            if (param2==NULL) return ERROR_SEMANTIC_VARIABLE;//nedefinovana promenna
        }
        else if(kw==LEX_NUMBER){
            param2=functionInsertConstant(table->currentFunc,gToken.data,kw);
        }
        else return ERROR_SYNTAX;

        if ((kw=getToken()) != LEX_R_SBRACKET)return ERROR_SYNTAX;
    }

    tInstr *i=genInstr(I_STRING,dest,param1,param2);
    err=listInsertLast(&(table->currentFunc->instructions),i);
    return err;
}

/**
 * @info      Analyza parametru definice fce
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsDefParams (tSymbolTable *table)
{
  // <params> - id <params_n>
  // <params> - eps
    tInstr *i;
    tKeyword kw;
    if ((kw = getToken()) == LEX_R_BRACKET) return ERROR_OK; //no params
    if (kw == LEX_ID){
        functionInsertSymbol(table->currentFunc,gToken.data);
        if (symbolTableSearchFunction(table,gToken.data)!=NULL)
            return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
        i=genInstr(I_POP,(getLastSymbol(table->currentFunc)),NULL,NULL);
        listInsertLast(&(table->currentFunc->instructions),i);
        return prsDefParamsN(table);
    }
  else return ERROR_SYNTAX;
}

/**
 * @info      Analyza dalsich parametru definice
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsDefParamsN (tSymbolTable *table)
{
  // <params_n> - , id <params_n>
  // <params_n> - eps
  tInstr *i;
  tKeyword kw;
  if ((kw = getToken()) == LEX_R_BRACKET) return ERROR_OK;
  if (kw != LEX_COMMA) return ERROR_SYNTAX;
  if (getToken() == LEX_ID) {
    if(functionInsertSymbol(table->currentFunc,gToken.data)==ERROR_INS_EXIST)
      return ERROR_SEMANTIC;//2 parametry se jmenuji stejne
    if (symbolTableSearchFunction(table,gToken.data)!=NULL)
      return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
    i=genInstr(I_POP,(getLastSymbol(table->currentFunc)),NULL,NULL);
    listInsertLast(&(table->currentFunc->instructions),i);
    return prsDefParamsN(table);
  }
  else return ERROR_SYNTAX;
}

/**
 * @info      Parsovani parametru pri volani fce
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsCallParams(tSymbolTable *table)
{
  tKeyword kw=getToken();
  tSymbol *symb;
  if (kw==LEX_R_BRACKET) return ERROR_OK; //no params
  Last(&(table->currentFunc->instructions));//aktivita na posledni instrukci
  if (kw == LEX_ID) {
    if ((symb=functionSearchSymbol(table->currentFunc, gToken.data)) == NULL){
      if (symbolTableSearchFunction(table,gToken.data)==NULL){
        return ERROR_SEMANTIC_VARIABLE;//predani nedefinovane promenne
      }
      else symb=functionInsertConstant(table->currentFunc, gToken.data, kw);//byla predana funkce
    }
  }
  else if (kw == LEX_NUMBER || kw == LEX_STRING || kw== KW_TRUE || kw== KW_FALSE || kw== KW_NIL){
    if ((symb=functionInsertConstant(table->currentFunc, gToken.data, kw)) == NULL)
        return ERROR_COMPILATOR; // chyba mallocu - nenastane
  }
  else return ERROR_SYNTAX;
  tInstr *i=genInstr(I_PUSH,symb,NULL,NULL);//pushnuti parametru
  postInsert(&(table->currentFunc->instructions),i);//instrukci vlozim za aktivni prvek
  return prsCallParamsN(table);
}

/**
 * @info      Parsovani dalsich parametru pri volani fce
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsCallParamsN(tSymbolTable *table)
{
  tKeyword kw;
  tSymbol *symb;
  if ((kw = getToken()) == LEX_R_BRACKET)
    return ERROR_OK; //zadne dalsi parametry
  else if (kw != LEX_COMMA)
    return ERROR_SYNTAX;

  if ((kw = getToken()) == LEX_ID) {
    if ((symb=functionSearchSymbol(table->currentFunc, gToken.data)) == NULL)
      return ERROR_SEMANTIC_VARIABLE;//predani nedefinovane promenne
  }
  else if (kw == LEX_NUMBER || kw == LEX_STRING || kw== KW_TRUE || kw== KW_FALSE || kw== KW_NIL){
    if ((symb=functionInsertConstant(table->currentFunc, gToken.data, kw)) == NULL)
      return ERROR_COMPILATOR; // error mallocu - nenastane
  }
  else return ERROR_SYNTAX;
  tInstr *i=genInstr(I_PUSH,symb,NULL,NULL);//pushnuti parametru
  postInsert(&(table->currentFunc->instructions),i);//instrukci vlozim za aktivni prvek
  return prsCallParamsN(table);
}


