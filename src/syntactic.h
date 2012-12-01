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

E_CODE parser (tSymbolTable *table);
E_CODE prsBody (tSymbolTable *table);
E_CODE prsCommand (tSymbolTable *table,tKeyword kw);
E_CODE prsDefFunction (tSymbolTable *table);
E_CODE prsStatlist (tSymbolTable *table);
E_CODE prsAssign (tSymbolTable *table);
E_CODE prsParams (tSymbolTable *table);
E_CODE prsParamsN (tSymbolTable *table);
E_CODE prsNum (tSymbolTable *table, tKeyword kw);
E_CODE prsTerm (tSymbolTable *table);
