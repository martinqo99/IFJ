#include <stdio.h>
#include "../src/mmu.h"
#include "../src/strings.h"
#include "../src/errors.h"
#include "../src/scanner.h"
#include "../src/expression.h"
#include "../src/syntactic.h"

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



    tSymbolTable table;
    symbolTableInit(&table);


    //if(table.mainFunc.instructions.first != NULL){printf("spatny init instr");exit;}

    err=parser(&table);
    printf("posledni token je:[%d:%d]%s\n",gToken.row,gToken.column,gToken.data.data);




    printf("err je : %d\n",err);
    

    mmuFclose(gFileHandler);
    
    mmuDump();
    
    mmuGlobalFree();
}
