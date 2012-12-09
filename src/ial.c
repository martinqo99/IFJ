/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    ial.c
 * Popis:     Soubor s fcemi do IAL
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "ial.h"
///////////////////////// Knuth-Moris-Prattuv algoritmus ///////////////////////
/**
 * @info      Vytvoreni pomocne tabulky
 * @param   tString - struktura s polem a jeho rozmery
 * @return  nic
 */

void kmpCrtTable (tString text, int* table){
  int position = 2, index = 0;

  table[0] = -1;
  table[1] = 0; // fixni hodnoty

  while (position < (int) text.len)
    if (text.data[position - 1] == text.data[index]) {
      index++;
      table[position] = index;
      position++; // KMP magic
    }
    else if (index > 0)
      index = table[index]; // KMP magic
    else {
      table[position] = 0;
      position++; // KMP magic
    }
}

/**
 * @info      Hledani podretezce v retezci
 * @param   tString - struktura s polem a jeho rozmery
 * @return  pozice prvniho stejneho znaku pri shode, jinak delka textu, ve kterem hledame
 */

int kmpSearch(tString text, tString searched){
    if(text.len == 0)
        return 0;

    int match = 0;
    int index = 0;
    int table[text.len];

    kmpCrtTable(text, table);

    while((match + index) < (int) text.len){
        if(searched.data[index] == text.data[match + index]){
            if(index == (int)searched.len - 1)
                return match; // nalezeno

            index++;
        }
        else{
            match += index - table[index]; // posun dale v prvnim

            if(table[index] > -1)
                index = table[index];
            else
                index = 0;
        }
    }

    return -1; // nenalezeno
}

/////////////////////// Quicksort //////////////////////////////////////////////

/**
 * @info      Radici fce quicksort
 * @param   char* - pole znaku
 * @param   int - zacatek a konec
 * @return  void
 */
void quicksort(char *array[], int left_begin, int right_begin)
{
  int pivot = (*array)[(left_begin + right_begin) / 2];
  int left_index = left_begin, right_index = right_begin, pom;
  do {
    while ((*array)[left_index] < pivot && left_index < right_begin)
      left_index++; // posun leveho indexu
    while ((*array)[right_index] > pivot && right_index > left_begin)
      right_index--; // posun praveho indexu

    if (left_index <= right_index) {
      pom = (*array)[left_index];
      (*array)[left_index] = (*array)[right_index];
      (*array)[right_index] = pom; // presun hodnot
      if (left_index < right_begin)
        left_index++;
      if (right_index > left_begin)
        right_index--;
    }
  } while (left_index < right_index); // nasleduje rekurzivni volani
  if (right_index > left_begin) quicksort(array, left_begin, right_index);
  if (left_index < right_begin) quicksort(array, left_index, right_begin);
}

////////////////// Tabulka symbolu pomoci binarniho stromu /////////////////////

int Max( int x, int y )
{
    return x > y ? x : y;
}

int Height(tBTNode N){
    if (N==NULL) return -1;
        else return N->height;
}
void btInit(tBTree *T){
    T->root=NULL;
    T->lastAdded=NULL;
}

void deleteNodes(tBTNode N){
    if(N->left!=NULL)deleteNodes(N->left);
    if(N->right!=NULL)deleteNodes(N->right);
    mmuFree(N);
}

void btFree(tBTree *T){
    deleteNodes(T->root);
    btInit(T);
}

tBTNode searchNodes(tBTNode N, tString *key){
    if(N==NULL) return NULL;
    else {
         int cmp=strCmp(key,N->key);
         if(cmp!=0){
             if(cmp<0)return searchNodes(N->left,key);
             else return searchNodes(N->right,key);
         }
         else return N;
    }
}

tBTNode btFind(tBTree *T,tString *key){
    return searchNodes(T->root,key);
}

E_CODE BTInsert (tBTree *T,tString *key, void *data) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
	if (T->root==NULL){ //prázdný strom
	        T->root=mmuMalloc(sizeof(struct tBTreeNode));
            T->root->key=key;
            T->root->data=data;
            T->root->left=T->root->right=NULL;
            T->root->height=0;
            T->lastAdded=T->root;
            return ERROR_OK;
	}
    tBTNode tmp = T->root;
    int cmpResult;
	while(tmp!=NULL){
            cmpResult=strCmp(key,tmp->key);
            if (cmpResult<0){
                if(tmp->left==NULL){
                //vlož levý
                tmp->left=mmuMalloc(sizeof(struct tBTreeNode));
                tmp->left->key=key;
                tmp->left->data=data;
                tmp->left->left=tmp->left->right=NULL;
                tmp->left->height=(tmp->height)+1;
                T->lastAdded=tmp->left;
                return ERROR_OK;
                }
                else tmp=tmp->left;//hledej dál vlevo
            }
            else if (cmpResult>0){
                if (tmp->right==NULL){
                //vlož pravý
                tmp->right=mmuMalloc(sizeof(struct tBTreeNode));
                tmp->right->key=key;
                tmp->right->data=data;
                tmp->right->left=tmp->right->right=NULL;
                tmp->right->height=(tmp->height)+1;
                T->lastAdded=tmp->right;
                return ERROR_OK;
                }
                else tmp=tmp->right;//hledej dál vpravo
            }
            else{ T->lastAdded=tmp;return ERROR_INS_EXIST; }
        }
    return ERROR_OK;
}

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
    strCopyString(&functionName,&(func->name)); // jmeno fce
    btInit(&(func->symbols)); // symboly
    initList(&(func->instructions)); // instrukce
    initList(&(func->constants));
    func->called=1;
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
