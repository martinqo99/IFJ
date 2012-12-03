#include <stdio.h>
#include "../src/mmu.h"
#include "../src/strings.h"
#include "../src/errors.h"
#include "../src/scanner.h"

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
    
    tKeyword keyword,kwNext;
    while((keyword = getToken()) != LEX_EOF){
        if(keyword == LEX_UNKNOWN){
            printf("Undefined lex\n");
            break;            
        }
        
        if(keyword == LEX_ERROR){
            printf("Lex error\n");
            break;
        }

        printf("- TOKEN [%d]: %s\n", keyword,gToken.data.data);   
        if(((kwNext=getTokenAhead())!=LEX_UNKNOWN) && (kwNext!=LEX_ERROR)) printf("pristi token je: %d\n",kwNext);     
    }
    
    mmuFclose(gFileHandler);
    
    mmuDump();
    
    mmuGlobalFree();
    
    return 0;    
}
