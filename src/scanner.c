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
            return LEX_RESERVED;        
    }
    
    for(int i = 0; i < KW_FALSE; i++){
         if(strCmpRaw(&word, keyWords[i]))
            return i;          
    }
    
    return S_ID;
}

void initToken(){
    gToken.row = 1;
    gToken.column = 0;
    
    rewind(gFileHandler);
    
    strInit(&(gToken.data));
}

void resetToken(){
    //strFree(&(gToken.data));
    strInit(&(gToken.data));
}

void pushToken(int character){
    strAdd(&(gToken.data), (char)character);
}

tKeyword getToken(){  
    //Reset tokenu do pocatecniho stavu
    resetToken();

    tState state = S_START;
    int c;
    
    //Hlavni nacitaci smycka
    while((c = getc(gFileHandler)) != EOF){
        //printf("[LEX] [%d:%d] Get character %c\n", gToken.row, gToken.column, c);

        //Posouvame cislo sloupce
        gToken.column++;
        
        if(c == '\n'){
            gToken.row++;
            gToken.column = 0;            
        }
        
        switch(state){
            case S_START:
                if(c == ' ' || c == '\r' || c == '\t') break;
                else if(c == EOF){ return LEX_EOF; }
                else if(c == '('){ pushToken(c); return LEX_L_BRACKET; }
                else if(c == ')'){ pushToken(c); return LEX_R_BRACKET; }
                else if(c == '['){ pushToken(c); return LEX_L_SBRACKET; }
                else if(c == ']'){ pushToken(c); return LEX_R_SBRACKET; }
                else if(c == '+'){ pushToken(c); return LEX_ADDITION; }
                else if(c == ','){ pushToken(c); return LEX_COMMA; }
                else if(c == ':'){ pushToken(c); return LEX_COLON; }

                //Cislo
                else if(isdigit(c)){ state = S_NUMBER; }
                //Identifikator
                else if(isalpha(c) || c == '_'){ state = S_ID; }
                //Unarni nebo binarni minus
                else if(c == '-'){ state = S_SUBSTRACTION; }
                //Deleni nebo komentar
                else if(c == '/'){ state = S_SLASH; }
                //Nasobeni nebo mocnina
                else if(c == '*'){ state = S_STAR; }
                //Nerovna se
                else if(c == '!'){ state = S_UNEQUAL; }
                //Mensi nez, nebo mensi nebo rovno
                else if(c == '<'){ state = S_LESSER; }
                //Vetsi nez, nebo vetsi nebo rovno
                else if(c == '>'){ state = S_GREATER; }
                //Retezec
                else if(c == '"'){ state = S_STRING; break; }
                //Prirazeni, porovnani
                else if(c == '='){ state = S_EQUAL; }             
                //Chybny znak
                else{ 
                    pushToken(c);
                    return LEX_UNKNOWN;                
                }
                
                pushToken(c);                
                break;
            case S_NUMBER:
                    if(c == EOF)
                        return LEX_ERROR;
                    else if(c == '.'){
                        state = S_NUMBER_POINT;
                        pushToken(c);                        
                        break;                        
                    }
                    else if(isdigit(c)){
                        pushToken(c);
                        break;                        
                    }
                    else
                        return LEX_NUMBER;
                break;
            case S_NUMBER_POINT:
                if(!isdigit(c))
                    return LEX_ERROR;
                
                state = S_NUMBER;
                pushToken(c);
                break;
            case S_ID:
                    if(c == EOF)
                        return LEX_ERROR;
                    else if(isdigit(c) || isalpha(c) || c == '_'){ 
                        pushToken(c); 
                        break;
                    }
                    else{ ungetc(c, gFileHandler); return isReserved(gToken.data); }
                break;
            case S_SUBSTRACTION:
                    return LEX_SUBSTRACTION;
                break;
            //Deleni, nebo komentare
            case S_SLASH:
                if(c == EOF)
                    return LEX_ERROR;
                else if(c == '/') 
                    state = S_COMMENT_ROW;
                else if(c == '*') 
                    state = S_COMMENT_BLOCK;
                else{ 
                    ungetc(c, gFileHandler); 
                    return LEX_DIVISION; 
                } 
                break;
            //Radkovy komentar
            case S_COMMENT_ROW:
                if(c == EOF)
                    return LEX_EOF;
                else if(c == '\n'){
                    state = S_START;
                    resetToken();
                }
                break;
            //Blokovy komentar
            case S_COMMENT_BLOCK:
                if(c == EOF) 
                    return LEX_ERROR;
                else if(c == '*')
                    state = S_COMMENT_END;
                break;
            //Konec blokoveho komentare
            case S_COMMENT_END:
                if(c == EOF) 
                    return LEX_ERROR;
                else if(c == '/'){
                    state = S_START;
                    resetToken();
                }
                else
                    state = S_COMMENT_BLOCK;
                break;
            //Mensi nez nebo mensi nebo rovno
            case S_LESSER:
                if(c == EOF) 
                    return LEX_ERROR;
                else if(c == '=') 
                    return LEX_LESSER_EQUAL;
                else{ 
                    ungetc(c, gFileHandler); 
                    return LEX_LESSER;
                }
                break;
            //Vetsi nez nebo vetsi nebo rovno
            case S_GREATER:
                if(c == EOF) 
                    return LEX_ERROR;
                else if(c == '=') 
                    return LEX_GREATER_EQUAL;
                else{ 
                    ungetc(c, gFileHandler); 
                    return LEX_GREATER; 
                }               
                break;
            //Retezec
            case S_STRING:
                    if(c == EOF)
                        return LEX_ERROR;
                    else if(c == '"')
                        return LEX_STRING;
                    else
                        pushToken(c);
                break;
            //Prirazeni nebo porovnani
            case S_EQUAL:
                    if(c == EOF) 
                        return LEX_ERROR;
                    else if(c == '=') 
                        return LEX_EQUAL;
                    else{ 
                        ungetc(c, gFileHandler); 
                        return LEX_ASSIGN;
                    }
                    return;
                break;
            //Nasobeni nebo mocnina
            case S_STAR:
                printf("star\n");
                if(c == EOF)
                    return LEX_ERROR;
                else if(c == '*'){
                    pushToken(c);
                    return LEX_POWER;                    
                }
                else{
                    ungetc(c, gFileHandler);
                    return LEX_MULTIPLICATION;
                }                    
                break;
            default:
                return LEX_ERROR;
                break;
        }
    }
}