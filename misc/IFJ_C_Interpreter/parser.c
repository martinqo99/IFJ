/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: parser.c - Syntakticky analyzator a generator kodu
 * Autor: Kamil Dudka, xdudka00
 */

#define INIT_TAPE_SIZE		256	// Pocatecni velikost pasky (pocet pseudoinstrukci)

#include "ifj05.h"
#include "scanner.h"
#include "symbols.h"
#include "variables.h"
#include "parser.h"
#include "expr.h"
#include "tape.h"
#include "runtime.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static char szErrString[] =		"Invalid string";
static char szErrEscape[] =		"Invalid escape sequence";
static char szErrChar[] =		"Invalid char in input";
static char szErrExcl[] =		"'=' expected after '!'";
static char szErrColon[] =		"':' or '=' expected after ':'";
static char szErrFloat[] =		"Invalid format of float";
static char szErrInt[] =		"Invalid format of integer";
static char szErrLabelRedefinition[] =	"Redefinition of label \"%s\"";
static char szErrSyntax[] =		"Syntax error";
static char szErrStatement[] =		"Unknown statement";
static char szErrSemicolon[] =		"Semicolon expected";
static char szErrNoId[] =		"Identifier expected";
static char szErrNoLabel[] =		"Label expected";
static char szErrEOF[] =		"Unexpected end of file";

/*
 * Generuje navesti (zapis do tabulky symbolu)
 */
static inline void GenLabel (PParserData pPData, void *id) {
	dprintf ("@%s::\n", SymbolsGetName (id));
	if (0== SymbolsSetLabel (id, TapeGetNextIndex (pPData->pTape)))
		// Ok..
		return;

	// Label redefinition (semantic error)
	bool bSkipErr = pPData -> bSkipErr;
	ParseError (pPData, szErrLabelRedefinition, SymbolsGetName (id));

	// Semantic error is not parser error
	pPData -> bSkipErr = bSkipErr;
}

/*
 * Konecny automat parsujici zdrojovy soubor
 */
static int MainFA (PParserData);

/*
 * Obsluha syntakticke chyby
 * 1. parametr - ukazatel na lokalni data parseru
 * 2. parametr - chybova hlaska bez "\n"
 */
void ParseError (PParserData pPData, ...) {
	// Zacatek funkce s promennym poctem parametru
	va_list pArgs;
	va_start (pArgs, pPData);

	// Nebude se generovat kod
	pPData-> bErrCompile = true;
	
	if (pPData-> bSkipErr)
		// Preskakuju chyby nez narazim na ';' (rizeno hlavnim FA)
		return;
	pPData-> bSkipErr  = true;

	// Vypis chybove hlasky
	GlobalErr ("%s:%d: error: ", pPData->srcFileName, pPData->token.line +1);
	
	// Vypis variabilni chybove hlasky
	const char *fmt = va_arg (pArgs, const char *);
	GlobalErrV (fmt, pArgs);
	
	// Odradkuje
	GlobalErr ("\n");

	// Konec funkce s promennym poctem parametru
	va_end (pArgs);
}

static inline void TokenCharErr (PParserData pPData) {
	// Nebude se generovat kod
	pPData-> bErrCompile = true;
	
	if (pPData-> bSkipErr)
		// Preskakuju chyby nez narazim na ';' (rizeno hlavnim FA)
		return;
	pPData-> bSkipErr  = true;

	// Vypis chybove hlasky
	GlobalErr ("%s:%d: error: ", pPData->srcFileName, pPData->token.line +1);
	
	// Vypis znaku ktery chybu zpusobil
	const int iChar = *(int *)(pPData->token.pData);
	if (EOF== iChar)
		GlobalErr ("EOF - ");

	else if (isalpha (iChar) || isdigit (iChar))
		GlobalErr ("'%c' - ", iChar);

	else
		GlobalErr ("0x%X - ", iChar);

	// Vypis vlastni chybove hlasky
	switch (pPData->token.tType) {
		case tInvalidString:
			GlobalErr (szErrString);
			break;
			
		case tInvalidEscape:
			GlobalErr (szErrEscape);			
			break;

		case tInvalidChar:
			GlobalErr (szErrChar);
			break;

		default:
			break;
	}
	
	// Odradkuje
	GlobalErr ("\n");
}


/*
 * Nacte dalsi token
 *   - vola funkci z modulu "scanner"
 *   - sam obslouzi lexikalni chybu
 * 1. parametr - ukazatel na lokalni data parseru
 */
int ParserGetNextToken (PParserData pPData) {
	ScannerGetNextToken (pPData->pScan, &(pPData->token));
	switch (pPData->token.tType) {	
		case tInvalidString:
		case tInvalidEscape:
		case tInvalidChar:
			TokenCharErr (pPData);
			return 1;

		case tInvalidExcl:
			ParseError (pPData, szErrExcl);
			return 1;

		case tInvalidColon:
			ParseError (pPData, szErrColon);
			return 1;

		case tInvalidSign:
		case tInvalidExp:
		case tInvalidFloat:
			ParseError (pPData, szErrFloat);
			return 1;

		case tInvalidInt:
			ParseError (pPData, szErrInt);
			return 1;
			
		default:
			return 0;
	}
}

/*
 * Vstupni bod Parseru - odstartuje syntaxi rizeny preklad a interpretaci kodu
 * 1. parametr - ukazatel na otevreny zdrojovy soubor (moznost presmerovani)
 * 2. parametr - jmeno zdrojoveho souboru - muze byt obsazeno v chybovych hlaskach
 * Vraci kod chyby (nula znamena vsechno OK)
 */
int ParserInvoke (FILE *hFile, const char *srcFileName) {
	// Lokalni data Parseru
	TParserData parserData;
		parserData.srcFileName= srcFileName,
		parserData.bErrCompile= false;
		parserData.bSkipErr = false;
	
	// Inicializace Scanneru
	if (NULL== (parserData.pScan = ScannerInit (hFile) ))
		GlobalErrAlloc ();

	// Inicializace Pasky (pro ukladani pseudokodu)
	if (NULL== (parserData.pTape = TapeCreate (INIT_TAPE_SIZE)))
		GlobalErrAlloc ();

	// Odstartuji syntaktickou analyzu
	dprintf ("Analysing source code:\n");
	int iErr = MainFA (&parserData);

	// Go!
	if (0==iErr) {
		dprintf ("\nSource ok, starting execution..\n");
		iErr = RuntimeProcess (parserData.pTape, srcFileName);
	}
       
#ifndef NDEBUG
	else
		dprintf ("One or more parse errors detected..\n");

	if (0==iErr)
		dprintf ("Normal program exit..\n");
#endif

	dprintf ("Destruction of tape..\n");
	TapeDestroy (parserData.pTape);
	
	dprintf ("Destruction of scanner..\n");
	ScannerDestroy (parserData.pScan);
	
	dprintf ("Thank you for using IFJ05..\n");
	return iErr;
}

/*
 * Zapise pseudoinstrukci na vystupni pasku
 * 1. parametr - odkaz na lokalni data parseru
 * 2. parametr - kod instrukce, ktera se ma vlozit
 * 3. parametr - ukazatel predany jako operand psudoinstrukce
 */
void Generate (PParserData pPData, TInstruction eInst, void *id) {
#ifndef NDEBUG
	switch (eInst) {
		case inPUSHV:
			dprintf ("  push from     %s\n", SymbolsGetName (id) );
			break;
		case inPUSHC:
			dprintf ("  push          "); ConstantPrint (id); dprintf ("\n");
			break;
		case inPOPV:
			dprintf ("  pop to        %s\n", SymbolsGetName (id) );
			break;
		case inREADV:
			dprintf ("  read          %s\n", SymbolsGetName (id) );
			break;
		case inPRINTV:
			dprintf ("  print         %s\n", SymbolsGetName (id) );
			break;
		case inJMPV:
			dprintf ("  jmp           @%s\n", SymbolsGetName (id) );
			break;
		case inJNZV:
			dprintf ("  jnz           @%s\n", SymbolsGetName (id) );
			break;
		case inCMPE:
			dprintf ("  cmp_e\n");
			break;
		case inCMPNE:
			dprintf ("  cmp_ne\n");
			break;
		case inCMPL:
			dprintf ("  cmp_l\n");
			break;
		case inCMPG:
			dprintf ("  cmp_g\n");
			break;
		case inCMPLE:
			dprintf ("  cmp_le\n");
			break;
		case inCMPGE:
			dprintf ("  cmp_ge\n");
			break;
		case inADD:
			dprintf ("  add\n");
			break;
		case inSUB:
			dprintf ("  sub\n");
			break;
		case inMUL:
			dprintf ("  mul\n");
			break;
		case inDIV:
			dprintf ("  div\n");
			break;
		default:
			GlobalErr ("Internal error: Invalid instruction\n");
			
	} // switch (eInst)
#endif
	TInstBlock inst;

	inst.Inst = eInst;
	inst.Params.pointer = id;
	inst.line_no = pPData->token.line;

	TapeAddInst (pPData->pTape, &inst);
}

/*
 * Hlavni FA syntaktickeho analyzatoru
 * Provadi syntaxi rizeny preklad
 *
 * 1. parametr - ukazatel na otevreny zdrojovy soubor (moznost presmerovani)
 * 2. parametr - jmeno zdrojoveho souboru - muze byt obsazeno v chybovych hlaskach
 * vraci: kod chyby (nula znamena vsechno OK)
 */
static int MainFA (PParserData pPData) {
	// Mnozina stavu FA, pocatecni stav je init
	enum TState {
		stInit,
		stIdReaded,
		stAssignReaded,
		stReadReaded,
		stPrintReaded,
		stGotoReaded,
		stOnReaded,
		stOnGotoReaded,
		stStatementReaded,
		stReadNext,
	} state = stInit;

	// Vlastni cyklus FA
	void *id = NULL;
	while (1) {
		ParserGetNextToken (pPData);
		switch (state) {
			case stReadNext:
				if (tEOF == pPData->token.tType)
					// Konec souboru - vsechno ok?
					return pPData->bErrCompile;

			case stInit:
				switch (pPData->token.tType) {
					case tIdentifier:
						id = pPData->token.pData;
						state = stIdReaded;
						break;
						
					case tKeyRead:
						state = stReadReaded;
						break;
						
					case tKeyPrint:
						state = stPrintReaded;
						break;
						
					case tKeyGoTo:
						state = stGotoReaded;
						break;
						
					case tKeyOn:
						state = stOnReaded;
						break;
						
					default:
						state = stStatementReaded;
						ParseError (pPData, szErrStatement);
				}
				break; // (stInit)
				
			case stIdReaded:
				switch (pPData->token.tType) {
					case tAssignment:
						state = stAssignReaded;
						break;

					case tLabel:
						state = stReadNext;
						GenLabel (pPData, id);
						break;

					default:
						state = stStatementReaded;
						ParseError (pPData, szErrSyntax);
				}
				break; // (stIdReaded)
				
			case stReadReaded:
				state = stStatementReaded;
				if (tIdentifier == pPData->token.tType)
					Generate (pPData, inREADV, pPData->token.pData);
				else
					// Za Read neni identifikator
					ParseError (pPData, szErrNoId);

				break;	// (stReadReaded)
								
			case stPrintReaded:
				state = stStatementReaded;
				
				if (tIdentifier == pPData->token.tType)
					Generate (pPData, inPRINTV, pPData->token.pData);
				else
					// Za Print neni identifikator
					ParseError (pPData, szErrNoId);

				break;	// (stPrintReaded)
				
			case stGotoReaded:
				state = stStatementReaded;

				if (tIdentifier == pPData->token.tType)
					Generate (pPData, inJMPV, pPData->token.pData);
				else
					// Za go_to neni identifikator
					ParseError (pPData, szErrNoLabel);

				break;	// (stGotoReaded)
				
			case stOnReaded:
				if (!ExprParse (pPData) && tKeyGoTo==pPData->token.tType)
					// Preskoci i klicove slovo go_to, ktere bylo nacteno behem PA
					state = stOnGotoReaded;
				else
					// Chyba ve vyrazu
					state = stStatementReaded;

				break;	// (stOnReaded)
				
			case stOnGotoReaded:
				state = stStatementReaded;

				if (tIdentifier == pPData->token.tType)
					Generate (pPData, inJNZV, pPData->token.pData);
				else
					// Za go_to neni identifikator
					ParseError (pPData, szErrNoLabel);

				break;	// (stOnGotoReaded)
				
			case stAssignReaded:
				// Zpracovani hodnoty na prave strane
				if (!ExprParse (pPData) && tSemicolon==pPData->token.tType) {
					// Preskoci i strednik, ktery byl nacten behem PA
					state = stReadNext;
					Generate (pPData, inPOPV, id);
					
					break; // (stAssignReaded)
				}

				// Chyba ve vyrazu
				state = stStatementReaded;
				
			case stStatementReaded:
				switch (pPData->token.tType) {
					case tSemicolon:
						state = stReadNext;

						// Zastavit preskakovani spatnych prikazu
						pPData-> bSkipErr = false;
						break;

					case tEOF:
						ParseError (pPData, szErrEOF);
						return 1;

					default:
						ParseError (pPData, szErrSemicolon);
				}

				break;	// (stStatementReaded)
				
		} // switch (state)

	} // while (1)
	return 0;
}

