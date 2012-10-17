/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: expr.h - Vyhodnocovani vyrazu (volano z modulu "parser")
 * Autor: Kamil Dudka, xdudka00
 */

#ifndef EXPR_H
#define EXPR_H

#include "parser.h"

/*
 * Zpracuje vyraz v zdrojovem souboru (volano z modulu "parser")
 * Pro obsluhu chyb pouziva funkci ParseError()
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * vraci: kod chyby (0 znamena ok)
 */
int ExprParse (PParserData);

#endif
