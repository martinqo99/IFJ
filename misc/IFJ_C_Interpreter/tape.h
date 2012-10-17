/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: tape.h - modul pasky instrukci
 * Autor: David Barina, xbarin02
 */

#ifndef TAPE_H
#define TAPE_H

#include "ifj05.h"
#include "symbols.h"
#include "variables.h"

/***
 * TInstructions
 *   seznam instrukci 
 */
typedef enum
        {
          // variable = symbol
          inPUSHV,  // s	variable => stack
          inPUSHC,  // c	constant => stack
          inPOPV,   // s	stack => variable
          inREADV,  // s	read => variable
          inPRINTV, // s	variable => print
          inJMPV,   // s	jmp @ variable
          inJNZV,   // s	if( stack != 0 ) jmp @ variable (+pop)
          inCMPE,   //    ( stack == stack(-1) ) => stack (+pop)
          inCMPNE,  //    ( stack != stack(-1) ) => stack (+pop)
          inCMPL,   //    ( stack <  stack(-1) ) => stack (+pop)
          inCMPLE,  //    ( stack <= stack(-1) ) => stack (+pop)
          inCMPG,   //    ( stack >  stack(-1) ) => stack (+pop)
          inCMPGE,  //    ( stack >= stack(-1) ) => stack (+pop)
          inADD,    //    ( stack +  stack(-1) ) => stack (+pop)
          inSUB,    //    ( stack -  stack(-1) ) => stack (+pop)
          inMUL,    //    ( stack *  stack(-1) ) => stack (+pop)
          inDIV,    //    ( stack /  stack(-1) ) => stack (+pop)
          inSORT    // n	heapsort( stack .. stack(-n+1) ) => stack (+pop)
        } TInstruction;

/***
 * TInstParams
 *   parametr instrukce 
 */
typedef union
        {
          void *pointer;      // c + s
//          PVariable constant; // c
//          PSymbol   symbol;   // s
          int       number;   // n ... parametr instrukce sort
        } TInstParams;

/***
 * TInstruction
 *   vlastni instrukce 
 */
typedef struct
        {
          TInstruction Inst;
          TInstParams Params;
          int line_no;
        } TInstBlock, *PInstBlock;

/***
 * TInstTape
 *   dynamicka paska instrukci
 */
typedef struct
        {
          PInstBlock insts;
          unsigned count;
          unsigned allocated;
          unsigned append;
        } TInstTape, *PInstTape;

/***
 * TapeCreate
 *   vytvori prazdnou pasku a vrati ukazatel na ni
 *   parametr - pocet instrukci pro pridani pri zvetsovani pasky
 */
PInstTape TapeCreate(unsigned append_size);

/***
 * TapeAddInst()
 *   prida instrukci do pasky (zkopiruje ji!)
 *   1. parametr - adresa pasky
 *   2. parametr - adresa instrukce (neprovede se na ni free)  
 */
void TapeAddInst(PInstTape tape, PInstBlock instruction);

/***
 * TapeGetNextIndex()
 *   vrati index dalsi instrukce v pasce (pro label)
 *   parametr - adresa pasky 
 */
unsigned TapeGetNextIndex(PInstTape tape);

/***
 * TapeDestroy
 *   uvolni pasku z pameti
 *   parametr - adresa pasky  
 */
void TapeDestroy(PInstTape tape);

#endif
