#include <stdio.h>
#include "../src/stack.h"

int main(){
	tStack stack;
	
	printf("Inicializace zasobniku\n");
	stackInit(&stack);
	
	printf("Uvolnovani zasobniku\n");
	stackFree(&stack);
	
	return 0;
}

