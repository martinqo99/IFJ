#include <stdio.h>
#include "../src/symbol_table.h"

int main(){
mmuInit();
tSymbolTable T;
symbolTableInit(&T);
tString str=strCreate("funkce");
symbolTableInsertFunction(&T,str);
tFunction *F=symbolTableSearchFunction(&T,str);
if(F==NULL)printf("chyba:nic se nenaslo"); else printf("OK:nalezena fce: %s\n",F->name.data);
if(symbolTableInsertFunction(&T,str)==ERROR_INS_EXIST) printf("OK:dvojita definice uspesne rozpoznana\n");
tString str2=strCreate("promenna");
functionInsertSymbol(F,str2);
tSymbol *S=functionSearchSymbol(F,str2);
if(S==NULL)printf("chyba:nic se nenaslo"); else printf("OK:nalezena promenna: %s\n",S->key.data);
if(F->symbols.lastAdded->data==S) printf("OK:posledni pridana promenna je rozpoznana\n");
    else printf("chyba: posledni pridana promenna se nerovna\n");
printf("OK: pristup k posledni pridane promenne:%s\n",((tSymbol*)(F->symbols.lastAdded->data))->key.data);
if(S==getLastSymbol(F))printf("OK:test fce getLastSymbol\n");else printf("chyba:fce getLastSymbol\n");
S=functionSearchSymbol(F,str);
if(S==NULL)printf("OK:nic se nenaslo\n"); else printf("chyba:nalezena promenna: %s\n",S->key.data);
S=functionInsertConstant(F,str,LEX_STRING); if(S==NULL) printf("chyba insert constant");
if (S==getLastConstant(F)) printf("getLastConstant je OK");
printf("posledni konstanta je:%s\n ",S->data->data.sData.data);
mmuGlobalFree();
}
