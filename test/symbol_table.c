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
    btInit(&(symbolTable->mainFunc.symbols));
    initList(&(symbolTable->mainFunc.constants));
    initList(&(symbolTable->mainFunc.instructions));
    btInit(&(symbolTable->functions));
    symbolTable->mainFunc.called=0;
    symbolTable->currentFunc=&(symbolTable->mainFunc);
}

E_CODE symbolTableInsertFunction(tSymbolTable* symbolTable, tString functionName){
    tFunction *func=mmuMalloc(sizeof(tFunction));
    strCopyString(&functionName,&(func->name));
    btInit(&(func->symbols));
    initList(&(func->instructions));
    initList(&(func->constants));
    func->called=-1;
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

tSymbol* insertBlankConstant(tFunction* function){
    tSymbol *symb=mmuMalloc(sizeof(tSymbol));
    symb->data=NULL;
    symb->key.data=NULL;
    listInsertLast(&(function->constants),symb);
    return symb;
}

tSymbol* getLastSymbol(tFunction* F){
    return (F==NULL ||F->symbols.lastAdded==NULL) ? NULL:(tSymbol*)(F->symbols.lastAdded->data);
}

tSymbol* getLastConstant(tFunction* F){
    return (F==NULL || F->constants.last==NULL) ? NULL:(tSymbol*)(F->constants.last->data);
}

tSymbol * functionInsertConstant(tFunction *function,tString data,tKeyword type){
if(function==NULL)return NULL;
    tSymbol *symb=mmuMalloc(sizeof(tSymbol));
    if (symb == NULL) return NULL;

    symb->data=mmuMalloc(sizeof(tSymbolData));
    if (symb->data == NULL) return NULL;

    switch(type){
        case LEX_STRING:{
            symb->data->type = DT_STRING;
            if (strCopyString(&data,&(symb->data->data.sData)) != ERROR_OK)
              {return NULL;}
        }
        break;
        case LEX_NUMBER:{
            char *endptr = NULL;
            symb->data->type = DT_NUMBER;
            symb->data->data.dData=strtod(data.data, &endptr); // ATOF SE NEPOUZIVA, je to HNUSNA fce
            if (*endptr != '\0' || strcmp(endptr, data.data) == 0) {
              //*err = ERROR_SYNTAX; // toto je nejspis neco jinyho, ty tu kua nemas err...a ani ho nemas jak vratit
              mmuFree(symb);
              return NULL;
            }
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
        case LEX_ID:{ //funkce pro TypeOf
            symb->data->type = DT_FUNCTION;
        }
        break;
        default: return NULL;
    }
    symb->key.data=NULL;
    listInsertLast(&(function->constants),symb);
    return symb;
}

tInstr* genInstr(tItype type, void *dest, void *src1, void *src2) {
   tInstr *inst = mmuMalloc(sizeof(tInstr));
   inst->type = type;
   inst->dest = dest;
   inst->src1 = src1;
   inst->src2 = src2;
   return inst;
}
