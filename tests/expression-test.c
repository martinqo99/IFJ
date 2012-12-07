#include <stdio.h>
#include "../src/mmu.h"
#include "../src/strings.h"
#include "../src/errors.h"
#include "../src/scanner.h"
#include "../src/expression.h"

int main(int argc, char* argv[]){

    //Inicializace MMU
    mmuInit(); 
    
    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
        return ERROR_COMPILATOR;
    }
    
    if(!(gFileHandler = mmuFopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open input file\n");
        return ERROR_COMPILATOR;
    }

    initToken(&gToken);
    E_CODE err=ERROR_OK;
    tString func=strCreate("function");
    tString x = strCreate("x");x=x;
    tSymbolTable table;
    symbolTableInit(&table);
    err=symbolTableInsertFunction(&table,func);
    table.currentFunc=symbolTableSearchFunction(&table,func);
    if(table.currentFunc==NULL)printf("funkce nenalezena");
    functionInsertSymbol(table.currentFunc,x);

    tSymbol *result=NULL;
    if(getToken()!=LEX_ID)printf("chyba: scanner nepoznal ID\n");
    if(getToken()!=LEX_ASSIGN)printf("chyba: scanner nepoznal =\n");
    tKeyword kw=getToken();kw=kw;
    err=prsExpression(&table,kw,&result);

    printf("err je : %d\n",err);
    if(result==NULL)printf("chyba: vysledek je null\n");

    mmuFclose(gFileHandler);
    
    mmuDump();
    
    mmuGlobalFree();
}
