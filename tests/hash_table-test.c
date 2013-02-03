#include <stdio.h>
#include "../src/strings.h"
#include "../src/hash_table.h"

#define KEYCOUNT 20

void htablePrint(tHTable* T){
    if(!T){
        printf("--- - Hash tabulka neni vytvorena\n");
        return;
    }

    if(!T->data){
        printf("--- - Hash tabulka je prazdna\n");
        return;
    }
    
    printf("--- - Hash tabulka je nadefinovana pro %d prvku\n", T->size);
}


int main(){
    tString* keys[KEYCOUNT];
    tString* key = NULL;

    for(int i = 0; i < KEYCOUNT; i++){
        key = malloc(sizeof(tString));
        
        strInit(key);
        
        keys[i] = key;
    }
    
    printf("#00 - Vytvoreni hash tabulky\n");
    tHTable* htable = htableCreate();
    htablePrint(htable);
    
    printf("#01 - Inicializace hash tabulky\n");
    htableInit(htable, 10);
    htablePrint(htable);
    
    //const char* key1 = "key1";
    //const char* key2 = "key1";
    
    //PPhtableLookup(htable);
    
    printf("#08 - Uvolnovani prvku hash tabulky\n");
    htableDispose(htable);
    
    printf("#09 - Zniceni hash tabulky\n");
    htableDestroy(htable);
    
    
    //printf("%d\n", hash("ahoj", 20));
    //printf("%d\n", hash("ahoj", 19));
    //printf("%d\n", hash("ahoj1", 20));
    //printf("%d\n", hash("ahoj1", 19));
    
    for(int i = 0; i < KEYCOUNT; i++){
        strFree(keys[i]);
        
        free(keys[i]);
    }

    return 0;
}
