/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka
 * Varianta: a/1/I
 * Soubor:   symbol_table.c
 *
 * Popis:
 *
 *
 * Datum:    20.11.2012
 *
 * Autori:   Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *           Matyas Petr         <xmatya03@stud.fit.vutbr.cz>
 *           Muzikarova Michaela <xmuzik04@stud.fit.vutbr.cz>
 *           Necasova Veronika   <xnecas21@stud.fit.vutbr.cz>
 *           Skacel Dalibor      <xskace11@stud.fit.vutbr.cz>
 */

#include "symbol_table.h"

void symbolTableInit(tSymbolTable* symbolTable){
    btInit(&(symbolTable->functions));
    symbolTable->currentFunc=&(symbolTable->mainFunc);
}

E_CODE symbolTableInsertFunction(tSymbolTable* symbolTable, tString functionName){
    tFunction *func=mmuMalloc(sizeof(tFunction));
    strCopyString(&functionName,&(func->name));
    btInit(&(func->symbols));
    initList(&(func->instructions));
    func->called=0;
    return BTInsert(&(symbolTable->functions),&(func->name),func);
}

tFunction* symbolTableSearchFunction(tSymbolTable* symbolTable, tString functionName){
    tBTNode tmp=btFind(&(symbolTable->functions),&functionName);
    return (tmp==NULL) ? NULL:(tFunction *)(tmp->data);
}

void symbolTableDispose(tSymbolTable* symbolTable){//je treba tyhle veci delat kdyz mame mmu?
    if (symbolTable==NULL) return;
    //projit stromem a zavolat functionDispose - zatim je tu memory leak
    btFree(&(symbolTable->functions));
    mmuFree(symbolTable);
    symbolTable=NULL;

}

tSymbol* functionSearchSymbol(tFunction *function, tString symbolname){
    tBTNode tmp=btFind(&(function->symbols),&symbolname);
    return (tmp==NULL) ? NULL:(tSymbol *)(tmp->data);
}

E_CODE functionInsertSymbol(tFunction* function,tString symbolname){
    tSymbol *symb=mmuMalloc(sizeof(tSymbol));
    strCopyString(&symbolname,&(symb->key));
    symb->type=DT_UNKNOWN;
    return BTInsert(&(function->symbols),&(symb->key),symb);    
}

tSymbol* getLastSymbol(tFunction* F){
    return (F==NULL ||F->symbols.lastAdded==NULL) ? NULL:(tSymbol*)(F->symbols.lastAdded->data);
}

E_CODE *genInstr(tFunction *F,tItype t, void *d, void *s1, void *s2) {
   TInstr *i = mmuMalloc(sizeof(TInstr));
   i->type = t;
   i->dest = d;
   i->src1 = s1;
   i->src2 = s2;
   return listInsertLast(F->currentFunc->instructions,i);

}
