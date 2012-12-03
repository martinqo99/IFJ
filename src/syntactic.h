/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    syntactic.h
 * Popis:     Hlavickovy soubor pro syntaktickou analyzu
 * Datum:     17.11.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "scanner.h"
#include "expression.h"

E_CODE parser (tSymbolTable*);
E_CODE prsBody (tSymbolTable*);
E_CODE findDefFunctions(tSymbolTable*);
E_CODE prsCommand (tSymbolTable*,tKeyword);
E_CODE prsDefFunction (tSymbolTable*);
E_CODE prsStatlist (tSymbolTable*);
E_CODE prsAssign (tSymbolTable*);
E_CODE prsDefParams (tSymbolTable*);
E_CODE prsDefParamsN (tSymbolTable*);
E_CODE prsNum (tSymbolTable*, tKeyword);
E_CODE prsTerm (tSymbolTable*);
