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
    if(err=findDefFunctions(table)!=ERROR_OK)return err;
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
    while(kw=getToken()!=LEX_EOF){
        if(kw==LEX_UNKNOWN || kw==LEX_ERROR || kw==LEX_RESERVED)
            return ERROR_LEX;
        if(kw==KW_FUNCTION){
            if(getToken()==LEX_ID){
                err=symbolTableInsertFunction(table,gToken->data);
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
        case default:{
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
    switch (kw){
        case LEX_ID:{
        //tady je treba pridat ID do tabulky, jestli tam uz neni
        //checkni =, zavolej prsAssign
            if(functionInsertSymbol(table->currentFunc,gToken->data)!=ERROR_INS_EXIST){
                //toto je prvni definice promenne
                if(symbolTableSearchFunction(table,gToken->data)!=NULL)
                    return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
            }
            if (getToken() != LEX_ASSIGN) return ERROR_SYNTAX;
            else if ((err = prsAssign(table)) != ERROR_OK) return err;
            break;
        }
        case KW_IF:{
        //checkni if/else vetev: if expression EOL <stat_list> else EOL <stat_list> end EOL
            if ((err = prsExpression(table, kw)) != ERROR_OK) return err;
            if ((err = prsStatlist(table)) != ERROR_OK) return err;
            if (getToken() != KW_ELSE) return ERROR_SYNTAX;
            if ((err=prsStatlist(table)) != ERROR_OK) return err; //end se nacte v prsStatlist
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_while:{
        //while loop: while expression EOL <stat_list> end EOL
            if ((err = prsExpression(table, kw)) != ERROR_OK) return err;
            if ((err = prsStatlist(table)) != ERROR_OK) return err;//end se nacte v prsStatlist
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_RETURN:{
        //return function: return expression EOL
            if ((err = prsExpression(table, kw)) != ERROR_OK) return err;
            break;
        }
        case default: return ERROR_SYNTAX;
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
    table->currentFunc=symbolTableSearchFunction(table,gToken->data);
    if (table->currentFunc==NULL)return ERROR_COMPILATOR;
    if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
    if ((err = prsDefParams(table)) != ERROR_OK) return err;//prava zavorka se nacte uvnitr params
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
    if ((err = prsCommand(table,kw)) == ERROR_OK) return prsStatlist(table); //@kw - prsCommand potrebuje znat posledni token
    else return err;
}

/**
 * @info      Analyza prirazovacich prikazu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsAssign (tSymbolTable *table)
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
    tKeyword kw, help;
    switch (kw = getToken()){
        case KW_INPUT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_R_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_NUMERIC:
        case KW_TYPEOF:
        case KW_LEN:{ //syntaxe u tehto 3 je stejna, ale nakonec se to bude muset rozdelit kvuli generovani rozdilnych instrukci myslim
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_ID) return ERROR_SYNTAX; //potreba check jestli ID existuje!
            if (getToken() != LEX_R_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_PRINT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if (err=prsTerm() != ERROR_OK) return err;
            if (getToken() != LEX_R_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_FIND:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if ((help = getToken()) != LEX_STRING || help != LEX_ID)
              return ERROR_SYNTAX;
            if (getToken() != LEX_COMMA) return ERROR_SYNTAX;
            if ((help = getToken()) != LEX_STRING || help != LEX_ID)
              return ERROR_SYNTAX;
            if (getToken() != LEX_R_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_SORT:{
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if ((help = getToken()) != LEX_STRING || help != LEX_ID)
              return ERROR_SYNTAX;
            if (getToken() != LEX_R_BRACKET) return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case LEX_STRING:{ // a co kdyz pujde o expression?
            if (getToken() != LEX_L_SBRACKET) return ERROR_SYNTAX;
            if ((help = getToken()) == LEX_NUMBER) {
              // prvni parametr stringu je cislo
              if ((err = prsNum(table, help)) != ERROR_OK) return err;
              if (getToken() != LEX_COLON) return ERROR_SYNTAX;
              if ((help = getToken()) == LEX_NUMBER) {
                // druhy taky
                if ((err = prsNum(table, help)) != ERROR_OK) return err;
                if (getToken() != LEX_R_SBRACKET) return ERROR_SYNTAX;
                if (getToken() != LEX_EOL) return ERROR_SYNTAX;
              }
              else if (help == LEX_R_SBRACKET);
              //druhy neni cislo
              else return ERROR_SYNTAX;
              if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            }
            else if (help == LEX_COLON) {
              // prvni parametru stringu neni cislo
              if ((help = getToken()) == LEX_NUMBER) {
                // druhy je cislo
                if ((err = prsNum(table, help)) != ERROR_OK) return err;
                if (getToken() != LEX_R_SBRACKET) return ERROR_SYNTAX;
                if (getToken() != LEX_EOL) return ERROR_SYNTAX;
              }
              else if (help == LEX_R_SBRACKET);
              // druhy neni cislo, to nevim jestli jde
              else return ERROR_SYNTAX;
              if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            }
            else return ERROR_SYNTAX;
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case LEX_ID:{// Dalibor: tady to chce check jestli je to ID funkce!
                      // jestli neni tak asi nebreakovat a pouzit to ID pro expression
                      // Kwisatz: jako by ses k tomu expression mohl dostat
            if (getToken() != LEX_L_BRACKET) return ERROR_SYNTAX;
            if ((err = prsParams()) != ERROR_OK) return err; //prava zavorka se checkne uz v prsParams
            if (getToken() != LEX_EOL) return ERROR_SYNTAX;
        }
        case default:{
            prsExpression(table, kw);
        }
    }
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

    tKeyword kw;
    if ((kw = getToken()) == LEX_R_BRACKET) return ERROR_OK;
    if (kw == LEX_ID){
        functionInsertSymbol(table->currentFunc,gToken->data);
        if (symbolTableSearchFunction(table,gToken->data)!=NULL) 
            return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
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

  tKeyword kw;
    if ((kw = getToken()) == LEX_R_BRACKET) return ERROR_OK;
    if (kw != LEX_COMMA) return ERROR_SYNTAX;
    if (getToken() == LEX_ID){
        functionInsertSymbol(table->currentFunc,gToken->data);
        if (symbolTableSearchFunction(table,gToken->data)!=NULL) 
            return ERROR_SEMANTIC;//promenna se jmenuje jako funkce
        return prsParamsN(table);
    }
    else return ERROR_SYNTAX;
}

/**
 * @info      Analyza cisla
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @param   tKeyword - klicove slovo
 * @return  E_CODE - chybovy kod
 */
E_CODE prsNum (tSymbolTable *table, tKeyword kw)
{
  // <num> - eps
  // <num> - num
  // pokud je kw == LEX_NUMBER, mame cislo uz nacteno a v tokenu
  // pokud ne, cislo musime nacist

  if (kw == LEX_NUMBER) // tohle by asi chtelo nejak vylepsit
    // doplneni tokenu do tabulky
    return ERROR_OK;
  else {
    if ((kw = getToken()) == LEX_NUMBER)
      //doplneni do tabulky
      return ERROR_OK;
    else if (kw == KW_END) return ERROR_OK;
    else return ERROR_SYNTAX;
  }
}

/**
 * @info      Analyza termu
 * @param   tSymbolTable* - ukazatel na tabulku znaku
 * @return  E_CODE - chybovy kod
 */
E_CODE prsTerm (tSymbolTable *table)
{
  // <term> - id
  // <term> - <value>
  // <value> - num
  // <value> - string
  // <value> - logic
  tKeyword kw;
  if ((kw = getToken()) == LEX_ID)
    // neco
    return ERROR_OK;
  else if (kw == LEX_NUMBER)
    return psrNum(table, kw);
  else if (kw = LEX_STRING)
    // tady se bude neco dit :D
    return ERROR_OK;
  else if (kw == KW_FALSE || kw == KW_TRUE)
    // doplnit
    return ERROR_OK;
  else return ERROR_SYNTAX;
}
