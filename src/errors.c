/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    errors.h
 * Popis:     
 * Datum:     16.11.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "errors.h"

E_CODE gErrorCode;

char* E_MESSAGES[] = {
    [ERROR_OK] = "Preklad probehl bez chyb"
    
};

void programAbort(E_CODE errorCode){
    gErrorCode = errorCode;
    
    exit(errorCode);    
}