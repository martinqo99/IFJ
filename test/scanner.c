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
    for(int i = 0; i < RESERVED_COUNT; i++)
        if(strcmp((char*)word.data, (char*)reservedWords[i]) == 0)
            return LEX_RESERVED;

    for(int i = 0; i < KW_FALSE; i++)
         if(strcmp(word.data, keyWords[i]) == 0)
            return i;

    return LEX_ID;
}

void initToken(){
    gToken.row = 1;
    gToken.column = 0;

    rewind(gFileHandler);

    strInit(&(gToken.data));
}

void resetToken(){
    strFree(&(gToken.data));
    strInit(&(gToken.data));
}

void pushToken(int character){
    strAdd(&(gToken.data), (char)character);
}

tKeyword getToken(){
    //Reset tokenu do pocatecniho stavu
    resetToken();

    tState state = S_START;
    int c, ascii_cnt;
    char ascii_val[2];

    //Hlavni nacitaci smycka
    while((c = getc(gFileHandler))){
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
                else if(c == '\n'){ pushToken(c); return LEX_EOL; }
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
            //Cislo - cela cast
            case S_NUMBER:
                    if(isdigit(c)){
                        pushToken(c);
                        break;
                    }
                    else if(c == '.'){
                        state = S_NUMBER_POINT;
                        pushToken(c);
                        break;
                    }
                    else if(c == 'e'){
                        state = S_NUMBER_EXPONENT;
                        pushToken(c);
                        break;
                    }
                    else
                        return LEX_ERROR;
                break;
            //Cislo - desetina cast
            case S_NUMBER_POINT:
                if(isdigit(c)){
                    pushToken(c);
                    break;
                }
                else if(c == 'e'){
                    state = S_NUMBER_EXPONENT;
                    pushToken(c);
                    break;
                }
                else if(isspace(c)){
                    ungetc(c, gFileHandler);
                    return LEX_NUMBER;
                }
                else if (c == '(' || c == ')' || c == '[' || c == ']' || c == '\n' ||
                           c == '+' || c == '-' || c == '*' || c == '/' ||
                           c == '=' || c == '!' || c == '<' || c == '>') {
                    ungetc(c, gFileHandler);
                    return LEX_NUMBER;
                }
                else
                    return LEX_ERROR;
                break;
            //Cislo - exponent
            case S_NUMBER_EXPONENT:
                if(c == '+' || c == '-'){
                    pushToken(c);
                    break;
                }
                else if(isdigit(c)){
                    pushToken(c);
                    break;
                }
                else if(isspace(c)){
                    ungetc(c, gFileHandler);
                    return LEX_NUMBER;
                }
                else if (c == '(' || c == ')' || c == '[' || c == ']' || c == '\n' ||
                           c == '+' || c == '-' || c == '*' || c == '/' ||
                           c == '=' || c == '!' || c == '<' || c == '>') {
                    ungetc(c, gFileHandler);
                    return LEX_NUMBER;
                }
                else
                    return LEX_ERROR;
                break;
            //Identifikator
            case S_ID:
                if(isdigit(c) || isalpha(c) || c == '_'){
                    pushToken(c);
                    break;
                }
                else if(c == EOF)
                    return LEX_ERROR;
                else{
                    ungetc(c, gFileHandler);
                    return isReserved(gToken.data);
                }
                break;
            //Minus
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
                    return LEX_EOL;
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
                    else if(c == '\\'){
                        state = S_STRING_ESCAPED;
                        //pushToken(c);
                    }
                    else if(c == '"')
                        return LEX_STRING;
                    else
                        pushToken(c);
                break;
            case S_STRING_ESCAPED:
                if(c == 'n'){
                    state = S_STRING;
                    pushToken('\n');
                }
                else if(c == 't'){
                    state = S_STRING;
                    pushToken('\t');

                }
                else if(c == '\\'){
                    state = S_STRING;
                    pushToken('\\');

                }
                else if(c == '"'){
                    state = S_STRING;
                    pushToken('\"');

                }
                else if (c == 'x') {
                    state = S_STRING_ASCII;
                    ascii_cnt = 0;
                }
                else
                    return LEX_ERROR;
                break;
            case S_STRING_ASCII:
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    if (ascii_cnt < 2)
                        ascii_val[ascii_cnt++] = c;
                    else {
                        ungetc(c, gFileHandler);
                        char *endptr = NULL;
                        long ascii_tmp = strtol(ascii_val, &endptr, 16);
                        if (*endptr != '\0' || strcmp(endptr, ascii_val) == 0)
                            return LEX_ERROR;
                        pushToken((int) ascii_tmp);
                        state = S_STRING;
                    }
                }
                else if (ascii_cnt == 2) {
                    ungetc(c, gFileHandler);
                    char *endptr = NULL;
                    long ascii_tmp = strtol(ascii_val, &endptr, 16);
                    if (*endptr != '\0' || strcmp(endptr, ascii_val) == 0)
                        return LEX_ERROR;
                    pushToken((int) ascii_tmp);
                    state = S_STRING;
                }
                else
                    return LEX_ERROR;
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
                break;
            case S_UNEQUAL:
                if(c == '='){
                    pushToken(c);
                    return LEX_UNEQUAL;
                }
                else
                    return LEX_ERROR;
                break;
            //Nasobeni nebo mocnina
            case S_STAR:
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

    return LEX_ERROR;
}

tKeyword getTokenAhead()
{
  unsigned int position = ftell(gFileHandler);
  tToken token = gToken;
  tString data;
  strCopyString(&gToken.data, &data);

  tKeyword kw = getToken();
  strFree(&gToken.data);

  token.data=data;
  gToken = token;

  fseek (gFileHandler, position, SEEK_SET);
  return kw;
}
