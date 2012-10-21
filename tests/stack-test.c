#include <stdio.h>
#include "../src/stack.h"

void printStack(tStack* S){
    if(!S){
        printf("--- - Zasobnik neni vytvoren\n");
        return;
    }

    if(!S->top){
        printf("--- - Zasobnik je prazdny\n");
        return;
    }

    tStackPtr iterator = S->top;
    int i = 0;
    do{
        printf("|%d| %d\n", ++i, *((int*)(iterator->data)));		

    }while((iterator = iterator->ptr));
}

int main(){
    tStack* stack = NULL;
    void* ptr = NULL;
    
    int a = 1;
    int b = 2;
    
    printf("#00 - Vytvoreni zasobniku\n");
    stack = stackCreate();

    printf("#01 - Inicializace zasobniku\n");
    stackInit(stack);
    printStack(stack);

    printf("#02 - Pocet polozek v zasobniku\n");
    printf("size = %d\n", stackSize(stack));

    printf("#03 - Pop nad prazdnym zasobnikem\n");
    stackPop(stack);
    printStack(stack);

    printf("#04 - Push 2 hodnot typu int\n");
    a = 1;
    stackPush(stack, &a);
    b = 2;
    stackPush(stack, &b);
    printStack(stack);

    printf("#05 - Ziskani prvni hodnoty pomoci stackTop\n");
    ptr = NULL;
    ptr = stackTop(stack);

    if(ptr)
        printf("a = %d\n", *((int*)(ptr)));
    else
        printf("a = NULL\n");

    printf("#06 - Ziskani prvni hodnoty pomoci stackPop\n");
    ptr = NULL;
    ptr = stackPop(stack);

    if(ptr)
        printf("a = %d\n", *((int*)(ptr)));
    else
        printf("a = NULL\n");

    printf("#07 - Ziskani prvni hodnoty pomoci stackTop\n");
    ptr = NULL;
    ptr = stackTop(stack);

    if(ptr)
        printf("a = %d\n", *((int*)(ptr)));
    else
        printf("a = NULL\n");

    printf("#08 - Uvolnovani prvku zasobniku\n");
    stackDispose(stack);
    
    printf("#09 - Zniceni zasobniku\n");
    stackDestroy(stack);

    return 0;
}
