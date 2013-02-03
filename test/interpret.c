/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    interpret.c
 * Popis:     Interpret prekladace jazyka IFJ12
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "interpret.h"

/**
 * @info      Interpret
 * @param   tInstr - instrukce v 3 adresnym kodu
 * @return  E_CODE - chybovy kod
 */
E_CODE interpret(tSymbolTable *table)
{

  E_CODE err;
  tStack *stack=stackCreate();
  err = stackInit(stack);
  err= interpret_recursive(&(table->mainFunc),stack);
  stackDispose(stack);
  stackDestroy(stack);
  return err;
}

E_CODE interpret_recursive (tFunction *function, tStack *stack)
{
  tListItem *help = function->instructions.first;
  E_CODE err = ERROR_OK;
    tInstr* instrPtr = NULL;
    tSymbol* src1SymbPtr = NULL;
    tSymbol* src2SymbPtr = NULL;
    tSymbol* destSymbPtr = NULL;
    tFunction* tmpFunc = NULL;
    tSymbolData* data1=NULL;
    tSymbolData* data2=NULL;
    tSymbolData* data3=NULL;
    int str_od,str_do;
    char*tmpstring;

  while (help != NULL && err == ERROR_OK) {
    instrPtr = (tInstr*) help->data;
    src1SymbPtr = (tSymbol*) instrPtr->src1;
    src2SymbPtr = (tSymbol*) instrPtr->src2;
    destSymbPtr = (tSymbol*) instrPtr->dest; // pomocny ukazatele

    switch (instrPtr->type) {printf("instrukce:%d",instrPtr->type);
      case I_RETURN: 
        return ERROR_OK;
        break;

      case I_CALL:
        tmpFunc=(tFunction*)instrPtr->dest;
        tmpFunc->called=tmpFunc->called+1;
        err=interpret_recursive(tmpFunc,stack);
        tmpFunc->called=tmpFunc->called-1;
        break;

      case I_LABEL:
        break;

      case I_POP:
        reallocSymbol(destSymbPtr,function);
        data1=(tSymbolData*)stackPop(stack);
        data2=getData(destSymbPtr,function);
        copySymbolData(data1,data2); 
        mmuFree(data1);

        break;

      case I_PUSH:
        //if (destSymbPtr==NULL)return ERROR_COMPILATOR;
        if (destSymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        data1=getData(destSymbPtr,function);          ;
        data2=mmuMalloc(sizeof(tSymbolData));
        err=copySymbolData(data1,data2);//nakopiruju data do data2 
        stackPush(stack,data2); 
        //printf("na scack jsem pushnul:%d",((tSymbolData*)stackTop(stack))->type);
        break;

      case I_SEMPTY:
            err = stackDispose(stack);
        break;

      case I_MOV:
            if (src1SymbPtr->data==NULL) return ERROR_SEMANTIC_VARIABLE;
            reallocSymbol(destSymbPtr,function);
            data1=getData(src1SymbPtr,function);
            data2=getData(destSymbPtr,function);
            copySymbolData(data1,data2);

        break;

      case I_ADD:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.dData = data1->data.dData + data2->data.dData;
            data3->type=DT_NUMBER;
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING) {

          data3->data.sData.data = mmuMalloc((data1->data.sData.len + data2->data.sData.len)*sizeof(char));

          strcat (data3->data.sData.data, data1->data.sData.data);
          strcat (data3->data.sData.data, data2->data.sData.data);
          data3->type=DT_STRING;
        }

        else if (data1->type == DT_STRING) {
          // tady nevim
            if(data2->type==DT_NUMBER){
            data3->data.sData.data = mmuMalloc((data1->data.sData.len + 20)*sizeof(char));    
            strcat (data3->data.sData.data, data1->data.sData.data);
            tmpstring=mmuMalloc(sizeof(char)*20);
            sprintf(tmpstring,"%lf",data2->data.dData);
            strcat (data3->data.sData.data, tmpstring);
            mmuFree(tmpstring);
            data3->type=DT_STRING;
            }
        }
        else return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_SUB:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        if(data1->type!=DT_NUMBER ||data2->type!=DT_NUMBER)return ERROR_INCOMPATIBLE_TYPE;
        data3->data.dData = data1->data.dData - data2->data.dData;
        data3->type=DT_NUMBER;

        break;

      case I_MUL:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.dData = data1->data.dData * data2->data.dData;
            data3->type=DT_NUMBER;
        }
        else if (src1SymbPtr->type == DT_STRING && src2SymbPtr->type == DT_NUMBER) {
          // n-ta mocnina retezce
          data3->data.sData.data = mmuMalloc((data1->data.sData.len * ((int)floor(data2->data.dData)))*sizeof(char));
          for(int i=0;i<(int)floor(data2->data.dData);i++) strcat(data3->data.sData.data,data1->data.sData.data);
        }
        else return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_DIV:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            if (data2->data.dData==0) return ERROR_ZERO_DIVISION;
            data3->data.dData = data1->data.dData / data2->data.dData;
            data3->type=DT_NUMBER;
        }
        else return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_POW:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER)
          data3->data.dData = pow(data1->data.dData, data2->data.dData);
        else return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_EQUAL:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);

        data3->type=DT_BOOL;
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
          data3->data.bData = (data1->data.dData == data2->data.dData);
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING){
          if (strcmp(data1->data.sData.data, data2->data.sData.data) == 0)
            data3->data.bData = true;
          else
            data3->data.bData = false;
        }
        else if (data1->type == DT_BOOL && data2->type == DT_BOOL){
          data3->data.bData = (data1->data.dData == data2->data.dData);
        }
        else if(data1->type==DT_NIL && data2->type==DT_NIL)
          data3->data.bData = true;
        else
          data3->data.bData = false;
        break;

      case I_NEQUAL:
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        data3->type=DT_BOOL;

        if(data1->type==data2->type){
            if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
                data3->data.bData = (data1->data.dData != data2->data.dData);
            }
            else if (data1->type == DT_STRING && data2->type == DT_STRING){
                if ((strcmp(data1->data.sData.data, data2->data.sData.data)) == 0)
                    data3->data.bData = false;
                else{
                    data3->data.bData = true;}
           }
           else if (data1->type == DT_BOOL && data2->type == DT_BOOL){
               data3->data.bData = (data1->data.dData != data2->data.dData);
           }
           else
              data3->data.bData = false;
        }
        else data3->data.bData = true;
        break;

      case I_LESS: // <
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        data3->type=DT_BOOL;
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.bData = (data1->data.dData < data2->data.dData);
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING){
          if (strcmp(data1->data.sData.data, data2->data.sData.data) < 0)
            data3->data.bData = true;
          else
            data3->data.bData = false;
        }
        else
          return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_ELESS: //<=
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        data3->type=DT_BOOL;
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.bData = (data1->data.dData <= data2->data.dData);
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING){
          if (strcmp(data1->data.sData.data, data2->data.sData.data) <= 0)
            data3->data.bData = true;
          else
            data3->data.bData = false;
        }
        else
          return ERROR_INCOMPATIBLE_TYPE;

        break;

      case I_MORE:  //>
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        data3->type=DT_BOOL;
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.bData = (data1->data.dData > data2->data.dData);
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING){
          if (strcmp(data1->data.sData.data, data2->data.sData.data) > 0)
            data3->data.bData = true;
          else
            data3->data.bData = false;
        }
        else
          return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_EMORE: //>=
        if(src1SymbPtr->data==NULL ||src2SymbPtr->data==NULL)return ERROR_SEMANTIC_VARIABLE;
        reallocSymbol(destSymbPtr,function);
        data1=getData(src1SymbPtr,function);
        data2=getData(src2SymbPtr,function);
        data3=getData(destSymbPtr,function);
        data3->type=DT_BOOL;
        if (data1->type == DT_NUMBER && data2->type == DT_NUMBER){
            data3->data.bData = (data1->data.dData >= data2->data.dData);
        }
        else if (data1->type == DT_STRING && data2->type == DT_STRING){
          if (strcmp(data1->data.sData.data, data2->data.sData.data) >= 0)
            data3->data.bData = true;
          else
            data3->data.bData = false;
        }
        else
          return ERROR_INCOMPATIBLE_TYPE;
        break;

      case I_JUMP:
        help=(tListItem*)instrPtr->dest;
        break;

      case I_FJUMP:
        data1=getData(src1SymbPtr,function);
        if (data1->type==DT_NUMBER && data1->data.dData==0.0)help=(tListItem*)instrPtr->dest;
        else if (data1->type==DT_BOOL && data1->data.bData==false)help=(tListItem*)instrPtr->dest;
        else if (data1->type==DT_STRING && (strcmp(data1->data.sData.data,"")==0))help=(tListItem*)instrPtr->dest;
        else if (data1->type==DT_NIL)help=(tListItem*)instrPtr->dest;
        break;

      case I_INPUT:
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        err=input(data1);
        stackDispose(stack); 
        break;

     case I_NUMERIC:
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data2=(tSymbolData*)stackPop(stack);
        err=numeric(data1,data2);
        stackDispose(stack); 
       break;
    case I_PRINT:  
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data1->type=DT_NIL;    
        while((data2=(tSymbolData*)stackPop(stack))!=NULL){
            print(*data2);
        }  

        break;
    case I_TYPEOF:
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data2=(tSymbolData*)stackPop(stack);
        typeOf(data1,data2);
        stackDispose(stack); 
       break;
    case I_LEN:
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data2=(tSymbolData*)stackPop(stack);
        len(data1,data2);
        stackDispose(stack); 
       break;
    case I_FIND:
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data2=(tSymbolData*)stackPop(stack);
        data3=(tSymbolData*)stackPop(stack);
        find(data1,data2,data3);
        stackDispose(stack); 
       break;
    case I_SORT:    
        reallocSymbol(destSymbPtr,function);
        data1=getData(destSymbPtr,function);
        data2=(tSymbolData*)stackPop(stack);
        err=sort(data1,data2);
        stackDispose(stack); 
        break;   
    case I_STRING:
        reallocSymbol(destSymbPtr,function);
        data3=(tSymbolData*)stackPop(stack);
        //data1=getData(src1SymbPtr,function);
        if(src1SymbPtr==NULL) str_od=0;else str_od=(int)floor(getData(src1SymbPtr,function)->data.dData);
        //data2=getData(src2SymbPtr,function);
        if(src2SymbPtr==NULL) str_do=strlen(data3->data.sData.data);else str_do=(int)floor(getData(src2SymbPtr,function)->data.dData);
        data1=getData(destSymbPtr,function);
        data1->type=DT_STRING;
        data1->data.sData.data=mmuMalloc((str_do*sizeof(char))+1);
        strncpy(data1->data.sData.data,data3->data.sData.data+str_od,str_do);
        data1->data.sData.data[str_do+1]='\0';
        break;
    default: return ERROR_COMPILATOR;         
    }//switch end
    help = help->next;

  }//while end
  return err;
}

bool isConstant(tSymbol *symb)
{

    return (symb->key.data==NULL)?TRUE:FALSE;
}

E_CODE copySymbolData(tSymbolData *src,tSymbolData *dest)
{
    if (src==NULL)return ERROR_SEMANTIC_VARIABLE;
    //if(dest==NULL)return ERROR_COMPILATOR;
    //dest=malloc(sizeof(tSymbolData));
    dest->type=src->type;
 
    if(src->type==DT_STRING){
        strCopyString(&(src->data.sData),&(dest->data.sData));
    }
    else dest->data=src->data;
    
    return ERROR_OK;
}
void reallocSymbol(tSymbol *symb,tFunction *F)
{
    tSymbolData *tmpdata;
    if(!isConstant(symb)){
        tmpdata=mmuRealloc(symb->data,(F->called+1)*sizeof(tSymbolData));
        symb->data=tmpdata;
    }
    else if (symb->data==NULL) symb->data=mmuMalloc(sizeof(tSymbolData));
}
tSymbolData* getData(tSymbol* symb,tFunction *F)
{
    
    if(isConstant(symb)) return &(symb->data[0]);
    else return &(symb->data[F->called]);

}
