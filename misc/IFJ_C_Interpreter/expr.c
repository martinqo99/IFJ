/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: expr.c - Zpracovani vyrazu (volano z modulu "parser")
 * Autor: Kamil Dudka, xdudka00
 */

#define STACK_INIT_SIZE		256	// Pocatecni velikost stacku (pocet polozek)

#include "ifj05.h"
#include "scanner.h"
#include "parser.h"
#include "expr.h"
#include "runtime.h"

#include <stdbool.h>
#include <stdlib.h>

static char szErrInternal[] =		"Internal error in module \"expr\"! Please, report bug to xdudka00@gmail.com";
static char szErrInvalidExpr[] =	"Invalid expression";
static char szErrHeapSort[] =		"Bad syntax of Heapsort call";
static char szErrRPar[] =		"Unexpected ')'";
static char szErrParenthesis[] =	"Expression error, check parenthesis!";
static char szErrOperand[] =		"Operand expected";
static char szErrOperator[] =		"Operator expected";

#define READEXPR_ERR_NOERR		0
#define READEXPR_ERR_HANDLED		1
#define READEXPR_ERR_RPAR		2

#ifdef NDEBUG
#	define DStackPrint(pStack)
#else
#	define DStackPrint(pStack)	StackPrint (pStack)
#endif

/*
 * Vycet hodnot, ktere se daji ulozit na zasobnik (nektere se tam ale neukladaji)
 */
typedef enum {
	PTEqual,	// =
	PTNotEqual,	// !=
	PTLess,		// <
	PTGreater,	// >
	PTLessEqual,	// <=
	PTGreaterEqual, // >=
	PTPlus,		// +
	PTMinus,	// -
	PTMultiply,	// *
	PTDivide,	// /
	PTParLeft,	// (
	PTParRight,	// )
	
	PTIdentifier,	// id
	PTInt,		// int
	PTFloat,	// float
	PTString,	// string
	
	PTSemicolon,	// ;
	PTGoTo,		// go_to
	PTComma,	// ,
	
	PTEq,		// ridici = (aplikuji na pravidlo '(=)' )
	PTLt,		// ridici <
	PTGt,		// ridici >
	PTExpr,		// Vyraz (rekurzivne)
	
	PTInv,		// Prazdna bunka - chyba
	PTEFce,		// prava zavorka navic (konec voleni fce?)

} EPTMember;

/*
 *  Precedencni tabulka (14 radku, 14 sloupcu) - viz. dokumentace
 */
static int pPTable [14][14] = {
/*		0	1	2	3	4	5	6	7	8	9	10	11	12	13	*/
/*		=	!=	<	>	<=	>=	+	-	*	/	(	)	i	$	*/
/* =  */	{PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* != */	{PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* <  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* >  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* <= */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* >= */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* +  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* -  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTLt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* *  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* /  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTLt,	PTGt,	PTLt,	PTGt,	},
/* (  */	{PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTEq,	PTLt,	PTInv,	},
/* )  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTInv,	PTGt,	PTInv,	PTGt,	},
/* i  */	{PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTGt,	PTInv,	PTGt,	PTInv,	PTGt,	},
/* $  */	{PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTLt,	PTEFce,	PTLt,	PTInv,	},
};

/*
 * Prevede zasobnikovy symbol na index do PA tabulky
 */
static inline unsigned PTGetIndex (unsigned i) {
	static const unsigned iPTI = 12;	// Identifikator ma v tabulce index 12
	static const unsigned iPTDollar = 13;	// Dollar ma v tabulce index 13

	if (i< PTInt)
		// Binarni operator
		return i;
	
	if (i< PTSemicolon)
		// Identifikator
		return iPTI;
	else
		// Dollar
		return iPTDollar;
}

/*
 * Vrati prvek PA tabulky na souradnicich i,j
 */
static inline EPTMember PTGetElement (unsigned i, unsigned j) {
	if (i > PTComma || j > PTComma) {
		//dprintf ("Table look-up: [%d,%d] - out of range!\n", i, j);
		return PTInv;
	}
/*	dprintf ("Table look-up: [%d,%d] = %d\n",
			PTGetIndex (i), PTGetIndex (j),
			pPTable [PTGetIndex (i)][PTGetIndex (j)]);*/
	return pPTable [PTGetIndex (i)] [PTGetIndex (j)];
}

/*
 * Prevede typ tokenu na zasobnikovy symbol
 */
static inline EPTMember TokenToEnum (TToken iToken) {
	switch (iToken) {
		case tOpEqual:		return PTEqual;		// =
		case tOpNotEqual:	return PTNotEqual;	// !=
		case tOpLess:		return PTLess;		// <
		case tOpGreater:	return PTGreater;	// >
		case tOpLessEqual:	return PTLessEqual;	// <=
		case tOpGreaterEqual:	return PTGreaterEqual;	// >=
		case tOpPlus:		return PTPlus;		// +
		case tOpMinus:		return PTMinus;		// -
		case tOpMultiply:	return PTMultiply;	// *
		case tOpDivide:		return PTDivide;	// /
		case tParenthesisLeft:	return PTParLeft;	// (
		case tParenthesisRight:	return PTParRight;	// )
		case tIdentifier:	return PTIdentifier;	// id
		case tLitInt:		return PTInt;		// int
		case tLitFloat:		return PTFloat;		// float
		case tLitString:	return PTString;	// string
		case tSemicolon:	return PTSemicolon;	// ;
		case tKeyGoTo:		return PTGoTo;		// go_to
		case tComma:		return PTComma;		// ,
		default:		return PTInv;		// chyba
	}
}

/*
 * Vraci true, pokud zasobnikovy symbol odpovida dolaru
 */
static inline bool IsTokenDollar (EPTMember e) {
	switch (e) {
		case PTSemicolon:
		case PTGoTo:
		case PTComma:
			return true;
		default:
			return false;
	}
}

/*
 * Vraci true, pokud je dany zasobnkovy symbol operand
 */
static inline bool IsTokenOperand (EPTMember e) {
	switch (e) {
		case PTIdentifier:
		case PTInt:
		case PTFloat:
		case PTString:
			return true;
		default:
			return false;
	}
}

/*
 * Vraci true, pokud je dany zasobnikovy symbol terminal
 */
static inline bool IsTerminal (EPTMember e) {
	return (e <= PTGoTo);
}

/*
 * Lokalni data zasobniku PA
 */
typedef struct {
	EPTMember *pStart;
	size_t size;
	size_t count;
	unsigned iTerm;
} TStack, *PStack;

/*
 * Prototypy funkci pracujicich ze zasobnikem PA
 */
static PStack StackInit (void);			// Inicializace stacku parseru
static void StackDestroy (PStack);		// Destrukce stacku parseru
static void StackPush (PStack, EPTMember);	// Vlozeni symbolu na zasobnik parseru
static void StackInsertLt (PStack);		// Vlozeni '<' do zasobniku parseru
static EPTMember StackPop (PStack);		// Vyjmuti symbolu na vrcholu zasobniku parseru
static EPTMember StackTopTerm (PStack);		// Vraci nejvrchnijsi TERMINAL na zasobniku

/*
 * Zpracuje vyraz v zdrojovem souboru
 * POZOR - Neprima rekurze pres funkci ReadSort()
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * vraci: kod chyby
 *   EXPR_ERR_NOERR		ok
 *   EXPR_ERR_HANDLED		obslouzena chyba
 *   EXPR_ERR_RPAR		chybi prava zavorka
 */
static int ReadExpr (PParserData);

/*
 * Nacte volani funkce Heapsort()
 * POZOR - Neprima rekurze pres funkci ReadExpr()
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * vraci: kod chyby (0 znamena vsechno OK)
 */
static int ReadSort (PParserData pPData);	// Nacte volani funkce Heapsort()

/*
 * VSTUPNI BOD MODULU EXPR
 * Zpracuje vyraz v zdrojovem souboru (volano z modulu "parser")
 * Pro obsluhu chyb pouziva funkci ParseError()
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * vraci: kod chyby (0 znamena ok)
 */
int ExprParse (PParserData pPData) {
	switch (ReadExpr (pPData)) {
		case READEXPR_ERR_NOERR:
			return 0;
			
		case READEXPR_ERR_RPAR:
			ParseError (pPData, szErrRPar);

		case READEXPR_ERR_HANDLED:
		default:
			return 1;
	}
}

/*
 * Zpracuje vyraz v zdrojovem souboru
 * Volano z modulu "parser", volano rekurzivne z funkce ReadSort() - jedna se neprimou rekurzi
 *
 * 1. parametr - ukazatel na lokalni data parseru
 * vraci: kod chyby (0 znamena vsechno OK)
 */
static int ReadExpr (PParserData pPData) {
	// Inicializaci stacku parseru
	// Stack alokoji pro kazdou instanci funkce ReadExpr() zvnou..
	PStack pStack = StackInit();

	// Informace o poslednim operandu
	TScannerToken tokenLastOp;
	tokenLastOp.tType = tInvalidChar;
	tokenLastOp.pData = NULL;
	tokenLastOp.line = 0;
	
	// Prevedeni typu tokenu na enumericky typ
	EPTMember eInput = TokenToEnum (pPData->token.tType);

	if (IsTokenOperand (eInput))
		// Ulozim si informace o poslednim operandu
		tokenLastOp = pPData->token;

	// Vlozim "$" na zasobnik - neco z mnoziny {';', 'go_to', ','} - takze treba ';'
	StackPush (pStack, PTSemicolon);

	// Hlavni cyklus precedencni analyzy
	do {
		if (tFuncSort== pPData->token.tType) {
			// Nacitam Heapsort
			if (0== ReadSort (pPData)) {
				// Nacten Heapsort
				eInput = TokenToEnum (pPData->token.tType);
				StackPush (pStack, PTExpr);

				if (IsTokenDollar (StackTopTerm (pStack)) && IsTokenDollar (eInput))
					// Heapsort je jedinym prvkem vyrazu
					break; // (do while)
			} else {
				// Chyba v syntaxi HeapSort
				ParseError (pPData, szErrHeapSort);
				StackDestroy (pStack);
				return READEXPR_ERR_HANDLED;
			}
		}

		// Klasicka PA
		switch (PTGetElement (StackTopTerm (pStack), eInput)) {
			case PTLt:
				// Vlozim '<' za posledni terminal
				StackInsertLt (pStack);
				
			case PTEq:
				// Vlozim symbol na zasobnik a nactu dalsi
				StackPush (pStack, eInput);
				ParserGetNextToken (pPData);
				eInput = TokenToEnum (pPData->token.tType);
				
				if (IsTokenOperand (eInput))
					// Ulozim si informace o poslednim operandu
					tokenLastOp = pPData->token;

				break;	// (PTLt, PTEq)

			case PTGt:
				// Redukce (aplikuju gramaticke pravidlo)

				if (IsTokenOperand (StackTopTerm (pStack)) ) {
					// Token je operand - odeberu ze zasobniku
					StackPop (pStack);

					// Odeberu '<' pokud tam je (melo by tam byt)
					if (PTLt!= StackPop (pStack)) {
						ParseError (pPData, szErrInvalidExpr);
						StackDestroy (pStack);
						return READEXPR_ERR_HANDLED;
					}

					// Nahrazeni terminalu non-terminalem
					StackPush (pStack, PTExpr);

					// Generovani kodu podle typu operandu
					switch (tokenLastOp.tType) {
						case tIdentifier:
							Generate (pPData, inPUSHV, tokenLastOp.pData);
							break;
						case tLitInt:
						case tLitFloat:
						case tLitString:
							Generate (pPData, inPUSHC, tokenLastOp.pData);
							break;
						default:
							// Jiny operand neni
							ParseError (pPData, szErrInternal);
							StackDestroy (pStack);
							return READEXPR_ERR_HANDLED;
					}
					break; // (PTGt)
				}

				if (PTParRight == StackTopTerm (pStack)) {
					// Redukuji zavorky
					StackPop (pStack);

					if (PTExpr!= StackPop (pStack) ||
							PTParLeft!= StackPop (pStack) ||
							PTLt!= StackPop (pStack)) {
						
						ParseError (pPData, szErrParenthesis);
						StackDestroy (pStack);
						return READEXPR_ERR_HANDLED;
					}
					dprintf ("  ; Drop brackets..\n");
					StackPush (pStack, PTExpr);
					break; // (PTGt)
				}
				
				// Predpoklada se binarni operace

				// Odeberu jeden perand
				if (PTExpr!= StackPop (pStack)) {
					ParseError (pPData, szErrOperand);
					StackDestroy (pStack);
					return READEXPR_ERR_HANDLED;
				}
				
				// obsluha binarniho operatoru
				switch (StackPop (pStack)) {
					case PTEqual:
						Generate (pPData, inCMPE, NULL);
						break;
					case PTNotEqual:
						Generate (pPData, inCMPNE, NULL);
						break;
					case PTLess:
						Generate (pPData, inCMPL, NULL);
						break;
					case PTGreater:
						Generate (pPData, inCMPG, NULL);
						break;
					case PTLessEqual:
						Generate (pPData, inCMPLE, NULL);
						break;
					case PTGreaterEqual:
						Generate (pPData, inCMPGE, NULL);
						break;
					case PTPlus:
						Generate (pPData, inADD, NULL);
						break;
					case PTMinus:
						Generate (pPData, inSUB, NULL);
						break;
					case PTMultiply:
						Generate (pPData, inMUL, NULL);
						break;
					case PTDivide:
						Generate (pPData, inDIV, NULL);
						break;
					default:
						ParseError (pPData, szErrOperator);
						StackDestroy (pStack);
						return READEXPR_ERR_HANDLED;
				}
				
				// Odeberu operand
				if (PTExpr!= StackPop (pStack)) {
					ParseError (pPData, szErrOperand);
					StackDestroy (pStack);
					return READEXPR_ERR_HANDLED;
				}

				if (PTLt!= StackPop (pStack)) {
					ParseError (pPData, szErrInvalidExpr);
					StackDestroy (pStack);
					return READEXPR_ERR_HANDLED;
				}
				
				// Nahrazeni binarni operace vyrazem
				StackPush (pStack, PTExpr);
				break;

			case PTEFce:
				// Neparova ')' - posledni parametr Heapsortu?

				if (IsTokenDollar (StackPop (pStack))) {
					// Nebyl zadan vyraz
					ParseError (pPData, szErrParenthesis);
					StackDestroy (pStack);
					return READEXPR_ERR_HANDLED;
				} else {
					StackDestroy (pStack);
					return READEXPR_ERR_RPAR;
				}

			default:
				ParseError(pPData, szErrInvalidExpr);
				StackDestroy (pStack);
				return READEXPR_ERR_HANDLED;
		}
	} while (!IsTokenDollar (StackTopTerm (pStack)) || !IsTokenDollar (eInput) );

	// All ok
	StackDestroy (pStack);
	return READEXPR_ERR_NOERR;
}

static int ReadSort (PParserData pPData) {
	int iParamsCnt = 0;

	// Nacteni '('
	ParserGetNextToken (pPData);
	if (tParenthesisLeft!= pPData->token.tType)
		return 1;

	// Nactu prvni token nasledujici za '('
	ParserGetNextToken (pPData);

	// Cyklus
	for (bool bReading = true; bReading; iParamsCnt++) {
		switch ( ReadExpr (pPData) ) {
			case READEXPR_ERR_RPAR:
				// $ je ')'
				bReading = false;
				break;

			case READEXPR_ERR_NOERR:
				// $ je symbol z mnoziny { ';', 'go_to', ',' }
				if (tComma== pPData->token.tType)
					break;
					
			case READEXPR_ERR_HANDLED:
			default:
				//dprintf ("  ; Error in Heapsort (E, ...)\n");
				return 1;
		}

		// Nactu dalsi token
		ParserGetNextToken (pPData);
	}

	// Vlozeni instrukce sort na vystupni pasku
	dprintf ("  sort          %d\n", iParamsCnt);
	TInstBlock inst;
	
	inst.Inst = inSORT;
	inst.Params.number = iParamsCnt;
	inst.line_no = pPData->token.line;

	TapeAddInst (pPData->pTape, &inst);

	//dprintf ("  ; Heapsort (E, ...) readed successful\n");
	return 0;
}

/*
 * Inicializace zasobniku parseru - (bude se volat pro kazdou instanci ReadExpr()
 */
static PStack StackInit (void) {
	// Alokuji prostor pro lokalni data zasobniku
	PStack pStack = malloc (sizeof(TStack));
	if (NULL== pStack)
		GlobalErrAlloc ();

	// Inicializace lokalnich dat
	pStack->count = 0;
	pStack->iTerm = 0;
	pStack->size = STACK_INIT_SIZE;

	// Alokace prostoru pro vlastni zasobnik
	if (NULL==( pStack->pStart = malloc (STACK_INIT_SIZE * sizeof(EPTMember)) ))
		GlobalErrAlloc ();

	// Vraci ukazatel na lokalni data
	return pStack;
}

/*
 * Destrukce zasobniku parseru - ke kazdemu StackInit() je potreba zavolat StackDestroy()
 */
static void StackDestroy (PStack pStack) {
	free (pStack-> pStart);
	free (pStack);
}

// Funkce vypisujici obsah zasobniku (pouze pro ladici ucely)
#ifndef NDEBUG
static inline void StackPrint (PStack pStack) {
	return;
	dprintf ("Stack: ");
	for (unsigned i=0; i< pStack->count; i++)
		printf ("%d ", pStack->pStart [i]);
	dprintf ("\n");
}
#endif

/*
 * Vlozi symbol na vrchol zasobniku
 */
static void StackPush (PStack pStack, EPTMember data) {
	if ( pStack->count >= pStack->size) {
		pStack->size <<= 1;
		dprintf ("  ; Change parser stack size to %d\n", pStack->size);
		if (NULL==( pStack->pStart = realloc (pStack->pStart, pStack->size * sizeof(EPTMember)) ))
			GlobalErrAlloc ();
	}
	pStack->pStart [pStack->count ++] = data;
	DStackPrint (pStack);
}

/*
 * Vyjme symbol z vrcholu zasobniku
 */
static EPTMember StackPop (PStack pStack) {
	pStack->count --;
	DStackPrint (pStack);
	return pStack->pStart [pStack->count];
}

/*
 * StackTopTerm() vraci nejvrchnejsi TERMINAL na zasobniku
 * Bude fungovat max. pro binarni operatory, coz nam bude stacit
 */
static EPTMember StackTopTerm (PStack pStack) {
	const EPTMember *stack = pStack->pStart;
	const size_t top = pStack->count -1;

	if (IsTerminal (stack [top]))
		return stack [top];
	else
		return stack [top -1];
}

/*
 * Vlozi '<' za posledni terminal
 */
static void StackInsertLt (PStack pStack) {
	if (IsTerminal (pStack->pStart [pStack->count -1]))
		StackPush (pStack, PTLt);
	else {
		EPTMember eTmp = StackPop (pStack);
		StackPush (pStack, PTLt);
		StackPush (pStack, eTmp);
	}
}

