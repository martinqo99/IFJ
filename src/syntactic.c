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
 * <def_function> - function idFunction (<params>) EOL <stat_list> EOL end EOL
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
 * <assign> - find(string, string)
 * <assign> - sort(string)
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
 * @info      Something
 * @param   parametry
 * @return  Whatever
 */
E_CODE parser(tSymbolTable *table){

return prsBody(table);
}

E_CODE prsBody(tSymbolTable *table){
    E_CODE err;
    tKeyword kw;
    while (kw=getToken()==LEX_EOL);
    switch (kw){
        case LEX_EOF: return ERROR_OK;
        case KW_FUNCTION:{ 
            if(err=prsDefFunction()!=ERROR_OK)return err;
            else return prsBody();
            }
        case default:{
            if(err=prsCommand()!=ERROR_OK)return err;
            else return prsBody();
            }
    }
}

E_CODE prsCommand(){
    E_CODE err;
    switch(getToken()){
        case LEX_ID:{
        //tady je treba pridat ID do tabulky, jestli tam uz neni
        //checkni =, zavolej prsAssign
            if (getToken()!=LEX_ASSIGN) return ERROR_SYNTAX;
            else if (err=prsAssign!=ERROR_OK) return err;
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_IF:{
        //checkni if/else vetev: if expression EOL <stat_list> else EOL <stat_list> end EOL
            if(err=prsExpression()!=ERROR_OK) return err;
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;
            if(err=prsStatlist()!=ERROR_OK) return err;
            if(getToken()!=KW_ELSE) return ERROR_SYNTAX;
            if(err=prsStatlist()!=ERROR_OK) return err;
            if(getToken()!=KW_END) return ERROR_SYNTAX; 
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;    
            break;       
        }
        case KW_while:{
        //while loop: while expression EOL <stat_list> end EOL
            if(err=prsExpression()!=ERROR_OK) return err;
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;            
            if(err=prsStatlist()!=ERROR_OK) return err;
            if(getToken()!=KW_END) return ERROR_SYNTAX; 
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;
            break;
        }
        case KW_RETURN:{
        //return function: return expression EOL
            if(err=prsExpression()!=ERROR_OK) return err;
            if(getToken()!=LEX_EOL) return ERROR_SYNTAX;  
            break;          
        }
        case default: return ERROR_SYNTAX;
    }
    return ERROR_OK;
}

E_CODE prsDefFunction(){
//function idFunction (<params>) EOL <stat_list> EOL end EOL

}

E_CODE prsStatlist(){
 /* <stat_list> - eps                        
 * <stat_list> - <command> <stat_list> */  
}

E_CODE prsAssign(){

}


