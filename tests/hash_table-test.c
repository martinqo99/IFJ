#include <stdio.h>
#include "../src/hash_table.h"


int main(){
    
    tHTable* htable = htableCreate();
    
    htableInit(htable, 10);
    
    htableDispose(htable);
    
    htableDestroy(htable);
    
    
    //printf("%d\n", hash("ahoj", 20));
    //printf("%d\n", hash("ahoj", 19));
    //printf("%d\n", hash("ahoj1", 20));
    //printf("%d\n", hash("ahoj1", 19));
 
    return 0;
}
