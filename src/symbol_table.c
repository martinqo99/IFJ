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
    E_CODE err=BTInsert(&(symbolTable->functions),&(func->name),func);
    if (err!=ERROR_OK){strFree(&(func->name));mmuFree(func);}
    return err;
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
    //symb->type=DT_UNKNOWN;
    symb->data=NULL;
    E_CODE err=BTInsert(&(function->symbols),&(symb->key),symb); 
    if (err!=ERROR_OK){strFree(&(symb->key));mmuFree(symb);}
    return err;   
}

tSymbol* getLastSymbol(tFunction* F){
    return (F==NULL ||F->symbols.lastAdded==NULL) ? NULL:(tSymbol*)(F->symbols.lastAdded->data);
}

E_CODE functionInsertConstant(tFunction *function,tString data,tKeyword type){
    tSymbol *symb=mmuMalloc(sizeof(tSymbol));
    symb->data=mmuMalloc(sizeof(tSymbolData));
    switch(type){
        case LEX_STRING:{
            symb->data->type = DT_STRING;
            strCopyString(&data,&(symb->data->data.sData));
        }
        break;
        case LEX_NUMBER:{
            symb->data->type = DT_NUMBER;
            symb->data->data.dData=atof(data.data);
        }
        break;
        case KW_NIL:{
            symb->data->type = DT_NIL;
        }
        break;
        case KW_TRUE:{
            symb->data->type = DT_BOOL;
            symb->data->data.bData = TRUE;
        }
        break;
        case KW_FALSE:{
            symb->data->type = DT_BOOL;
            symb->data->data.bData = FALSE;
        }
        break;
        default: return ERROR_COMPILATOR;
    }
    symb->key.data=NULL;
    return listInsertLast(&(function->constants),symb);
}

tInstr* genInstr(tItype type, void *dest, void *src1, void *src2) {
   tInstr *inst = mmuMalloc(sizeof(tInstr));
   inst->type = type;
   inst->dest = dest;
   inst->src1 = src1;
   inst->src2 = src2;
   return inst;
}
