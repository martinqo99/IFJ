/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   scanner.c
 * 
 * Popis:    
 * 
 * 
 * Datum:    21.11.2012
 * 
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#include "scanner.h"

FILE* gFileHandler = NULL;
tToken gToken;

const char* keyWords[] = {
    [KW_IF] = "if", [KW_ELSE] = "else", [KW_END] = "end",
    [KW_WHILE] = "while",
    [KW_FUNCTION] = "function", [KW_RETURN] = "return",
    [KW_FIND] = "find", [KW_SORT] = "sort", [KW_INPUT] = "input", [KW_NUMERIC] = "numeric", [KW_PRINT] = "print", [KW_TYPEOF] = "typeOf", [KW_LEN] = "len",
    [KW_NIL] = "nil", [KW_TRUE] = "true", [KW_FALSE] = "false",
};

const char* reservedWords[] = { "import", "def", "export", "from", "as", "directive", "launch", "load", "macro" };

int isReserved(tString word){
    for(int i = 0; i < RESERVED_COUNT; i++){
        if(strCmpRaw(&word, reservedWords[i]))
            return i;        
    }
    
    for(int i = 0; i < KW_FALSE; i++){
         if(strCmpRaw(&word, keyWords[i]))
            return i;          
    }
    
    return S_ID;
}

void initToken(){
    gToken.state = S_START;
    gToken.row = 1;
    gToken.column = 0;    
    strInit(&(gToken.data));
}

void resetToken(){
    gToken.state = S_START;
    strFree(&(gToken.data));
    strInit(&(gToken.data));
}

void setState(tState state){
    gToken.state = state;
}

void pushToken(int character){
    strAdd(&(gToken.data), (char)character);
}

tState getToken(){  
    if(!gFileHandler)
        programAbort(ERROR_COMPILATOR);
    
    if(gToken.state == S_EOF)
        programAbort(ERROR_LEX);
    
    //Reset tokenu do pocatecniho stavu
    resetToken();

    int c;
    //Hlavni nacitaci smycka
    while((c = getc(gFileHandler)) != EOF){
        printf("[LEX] [%d:%d] Get character %c\n", gToken.row, gToken.column, c);

        //Posouvame cislo sloupce
        gToken.column++;
        
        if(c == '\n'){
            gToken.row++;
            gToken.column = 0;            
        }
        
        switch(gToken.state){
            case S_START:
                //Bile znaky
                if(isspace(c)){ 
                    break;
                }

                //else if(c == EOF){ return EOF; }
                else if(c == '('){ return LEX_L_BRACKET; }
                else if(c == ')'){ return LEX_R_BRACKET; }
                else if(c == '['){ return LEX_L_SBRACKET; }
                else if(c == ']'){ return LEX_R_SBRACKET; }
                else if(c == '+'){ return LEX_ADDITION; }
                else if(c == ','){ return LEX_COMMA; }
                else if(c == ':'){ return LEX_COLON; }

                //Cislo
                else if(isdigit(c)){ setState(S_NUMBER); }
                //Identifikator
                else if(isalpha(c)){ setState(S_ID); }
                else if(c == '_'){ setState(S_ID); }
                
                //Unarni nebo binarni minus
                else if(c == '-'){ setState(S_SUBSTRACTION); }
                //Deleni nebo komentar
                else if(c == '/'){ setState(S_DIVISION); }
                //Nasobeni nebo mocnina
                else if(c == '*'){ setState(S_ADDITION); }
                //Nerovna se
                else if(c == '!'){ setState(S_UNEQUAL); }
                //Mensi nez, nebo mensi nebo rovno
                else if(c == '<'){ setState(S_LESSER); }
                //Vetsi nez, nebo vetsi nebo rovno
                else if(c == '>'){ setState(S_GREATER); }
                //Literal
                else if(c == '\''){ setState(S_STRING); }
                //Retezec
                else if(c == '"'){ setState(S_STRING); }
                //Prirazeni, porovnani
                else if(c == '='){ setState(S_EQUAL); }             
                //Chybny znak
                else{ 
                    printf("[LEX] Unallowed character in S_START: %c\n", c);
                    programAbort(ERROR_LEX);                    
                }
                
                pushToken(c);                
                break;
            case S_NUMBER:
                    printf("[LEX] Number\n");
                    return;
                break;
            case S_ID:
                    printf("[LEX] ID\n");
                    return;
                break;
            case S_SUBSTRACTION:
                    printf("[LEX] Substraction\n");
                    return;
                break;
            //Deleni, nebo komentare
            case S_DIVISION:
                if(isdigit(c)){ ungetc(c, gFileHandler); return LEX_DIVISION; }                
                else if(c == '/') setState(S_COMMENT_ROW);
                else if(c == '*') setState(S_COMMENT_BLOCK);
                else programAbort(ERROR_LEX);
                break;
            //Radkovy komentar
            case S_COMMENT_ROW:
                if(c == '\n')
                    setState(S_START);
                break;
            //Blokovy komentar
            case S_COMMENT_BLOCK:
                if(c == '*')
                    setState(S_COMMENT_END);
                break;
            //Konec blokoveho komentare
            case S_COMMENT_END:
                if(c == '/')
                    setState(S_START);
                else
                    setState(S_COMMENT_BLOCK);
                break;
            //Mensi nez nebo mensi nebo rovno
            case S_LESSER:
                if(c == '=') return LEX_LESSER_EQUAL;
                else{ ungetc(c, gFileHandler); return LEX_LESSER; }
                break;
            //Vetsi nez nebo vetsi nebo rovno
            case S_GREATER:
                if(c == '=') return LEX_GREATER_EQUAL;
                else{ ungetc(c, gFileHandler); return LEX_GREATER; }               
                break;
            case S_STRING:
                    printf("[LEX] String\n");
                    return;
                break;
            case S_EQUAL:
                    printf("[LEX] Equal\n");
                    return;
                break;
            default:
                programAbort(ERROR_LEX);
                break;
        }
    }
}
    
    /*
    int i = 0;		// pocitadlo prectenych znaku

    //spusteni konecneho automatu
    while ((c = getc(gFileHandler))){
        switch (stav){
            case S_START:		// 0 - pocatecni stav
                //DOPSAT BOOL a NUMERIC - funkce isdigit??
                if(c == '=')       stav = S_PRIRAZENI;
                else if(c == '.')  stav = S_TECKA;
                else if(c == '(')  stav = S_LZ;
                else if(c == ')')  stav = S_PZ;
                else if(c == ';')  stav = S_STREDNIK;
                else if(c == ',')  stav = S_CARKA;
                else if(c == '!')  stav = S_VYKRICNIK;
                else if(c == '+')  stav = S_SOUCET;
                else if(c == '-')  stav = S_ROZDIL;
                else if(c == '*')  stav = S_SOUCIN;
                else if(c == '/')  stav = S_PODIL;
                // Nutne predelat - v int c nemuzou byt dva znaky! Zavisle na predchozim znaku
                else if (c == '**') stav = S_MOCNINA;
                else if (c == '<')  stav = S_MENSI;
                else if (c == '>')  stav = S_VETSI;
                else if (c == '<=') stav = S_MENSIROVNO;
                else if (c == '>=') stav = S_VETSIROVNO;
                else if (c == '!=') stav = S_NEROVNASE;
                else if (c == '==') stav = S_ROVNASE;
                //
                else if (c == '"') stav = S_STR;
                else if (c == ' '){
                    stav = S_START;
                    break;
                }
                
                rozsirToken(c, &i);	//vlozeni znaku do token.data
                break;
                //SEM JE POTREBA DOPSAT CHYBU PRO PRIPAD, ZE ZNAK NEODPOVIDA ANI JEDNOMU
            case S_SOUCET:
            case S_ROZDIL:
            case S_SOUCIN:
            case S_PODIL:
                token.stav = stav;
                stav = S_END;
                break;
            case S_MENSI:
                if (c == '=') {
                    stav = S_MENSIROVNO;
                    rozsirToken(c, &i);
                }
                else {
                    naplnToken(stav);
                    stav = S_END;
                }
                break;
            case S_VETSI:
                if (c == '=') {
                    stav = S_VETSIROVNO;
                    rozsirToken(c, &i);
                }
                else {
                    naplnToken(stav);
                    stav = S_END;
                }
                break;
            //Chybi dopsat zbyvajici stavy! 
            default:
                break;
        } //konec while
    }
    */