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

#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#include <stdlib.h>

typedef enum e_code{
    ERROR_OK                = 0,
    ERROR_LEX               = 1,
    ERROR_SYNTAX            = 2,
    ERROR_SEMANTIC_VARIABLE = 3,
    ERROR_SEMANTIC_FUNCTION = 4,
    ERROR_SEMANTIC          = 5,
    ERROR_ZERO_DIVISION     = 10,
    ERROR_INCOMPATIBLE_TYPE = 11,
    ERROR_NUMERIC_CONVERSION= 12,
    ERROR_RUNTIME           = 13,
    ERROR_COMPILATOR        = 99,
    
    //Development errors
    ERROR_INS_EXIST,
    ERROR_ELSE
} E_CODE;

extern E_CODE gErrorCode;

void programAbort(E_CODE);

#endif
