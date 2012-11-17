/**
 * Projekt: Implementace interpretu jazyka IFJ12
 * Soubor: lexikalni_analyzator.c
 * Autori: Frantisek Kolacek
 *         Petr Matyas
 *         Dalibor Skacel
 *         Michaela Muzikarova
 *         Veronika Necasova
**/

  // prilozeni hlavickovych souboru
#include "scanner.h"

  // globalni promenne
tToken token;

void initToken(tToken* token){
  token->state = S_START;
  token->nextState = S_START;
  token->line = 0;
  token->column = 0;
  
  strClear(&(token->data));
}

void pushToken(tToken* token, int character){
    strAdd(&(token->data), (char)character);
}

void getToken(tToken* token){
    
    int c;
    int lines = 0;
    int columns = 0;
    tStav state = token->nextState;

    while((c = getc(gFileHandler))){
        columns++;
        
        switch(state){
            case S_START:            
                switch(c){
                    case '\n': columns = 0; lines++;
                    
                    case '.': state = S_TECKA; break;
                    case '(': state = S_LZ; break;
                    case ')': state = S_PZ; break;
                    
                    case ';': state = S_STREDNIK; break;
                    case '!': state = S_VYKRICNIK; break;
                    
                    case '=': state = S_PRIRAZENI; break;
                    case '<': state = S_MENSI; break;
                    case '>': state = S_VETSI; break;
                    
                    case '+': state = S_SOUCET; break;
                    case '-': state = S_ROZDIL; break;
                    case '*': state = S_SOUCIN; break;
                    case '/': state = S_PODIL; break;
                    
                    case '"': state = S_STR; break;
                    case ' ':                        
                        break;
                    default:
                        break;
                }
                
                pushToken(token, c);           
                break;
            case S_SOUCET:
            case S_ROZDIL:
            case S_SOUCIN:
            case S_PODIL:
                break;            
            case S_MENSI:
                //<=
                if(c == '='){
                    state = S_MENSIROVNO;
                    pushToken(token, c);
                }
                else
                    state = S_END;
                break;
            case S_VETSI:
                //>=
                if(c == '='){
                    state = S_VETSIROVNO;
                    pushToken(token, c);
                }
                else
                    state = S_END;
                break;
            //Nepovolena sekvence -- toto nesmi nastav, ale pro cistotu kodu!
            default:
                break;
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
}

