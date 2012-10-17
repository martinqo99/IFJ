/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: ifj05.h - obsluha chyb
 * Autor: Kamil Dudka, xdudka00
 */

#ifndef IFJ05_H
#define IFJ05_H

#include <stdarg.h>

/*
 * dprintf(...) - ladici vypisy na stdout pokud neni definovano makro NDEBUG
 */
#ifdef NDEBUG
#define dprintf(...)
#else
#define dprintf(...) printf (__VA_ARGS__)
#endif

/*
 * Tisk chybove hlasky
 * Parametry jsou stejne jako u funkce printf()
 */
void GlobalErr (const char *, ...);

/*
 * Tisk chybove hlasky
 * Parametry jsou stejne jako u funkce vprintf()
 */
void GlobalErrV (const char *, va_list);

/*
 * Vola se pri neuspesne alokaci
 * Shodi prekladac
 */
void GlobalErrAlloc (void);

#endif
