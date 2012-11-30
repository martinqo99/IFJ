#include <stdio.h>
#include "../src/mmu.h"
#include "../src/strings.h"

int main(){
    
    mmuInit();
    
    tString text;
    
    strInit(&text);
    
    for(int i = 'A'; i <= 'z'; i++){
        strAdd(&text, (char)i);
        
        printf("%s\n", text.data);
        
    }
    
    mmuDump();
    
    mmuGlobalFree();
    
    return 0;
}