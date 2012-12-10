/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    expression.c
 * Popis:     Vyhodnoceni vyrazu
 * Datum:     1.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include"expression.h"
const char PrecedentTable [MAXTABLE][MAXTABLE] =
{ // tabulka nemusi byt dobre, chce se to nekomu kontrolovat?
  // tokeny                         id  (   )   +   -   *   /   **  ==  !=  <   <=  >   >=
  [LEX_ID]              ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_L_BRACKET]       ={[LEX_ID]='<','<','=','<','<','<','<','<','<','<','<','<','<','<',[LEX_EOL]= 0 },
  [LEX_R_BRACKET]       ={[LEX_ID]= 0 , 0 ,'>','>','>','>','>','>','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_ADDITION]        ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_SUBSTRACTION]    ={[LEX_ID]='<','<','>','>','>','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_MULTIPLICATION]  ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_DIVISION]        ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_POWER]           ={[LEX_ID]='<','<','>','>','>','>','>','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_EQUAL]           ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_UNEQUAL]         ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_LESSER]          ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_LESSER_EQUAL]    ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_GREATER]         ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_GREATER_EQUAL]   ={[LEX_ID]='<','<','>','<','<','<','<','<','>','>','>','>','>','>',[LEX_EOL]='>'},
  [LEX_EOL]             ={[LEX_ID]='<','<', 0 ,'<','<','<','<','<','<','<','<','<','<','<',[LEX_EOL]='$'},
};


E_CODE prsExpression(tSymbolTable *table,tKeyword kw,tSymbol **result)
{
    E_CODE err=ERROR_OK;
    tKeyword a,b;
    char c;
    tExprData *tmpData;
    tSymbol *dest=NULL,*sourc1=NULL,*sourc2=NULL;
    tInstr *i;
    tItype itype=NOINSTR;
    tStack* stack=stackCreate();
    stackInit(stack);


    stackPush(stack,(newExprdata(LEX_EOL,NULL)));//vlozi $ na zasobnik
    do{
        a=topTerm(stack);
        b=kw;//printf("a=%d,b=%d\n",a,b);
        if(b==LEX_ID || b==LEX_STRING || b==LEX_NUMBER ||kw==KW_NIL||kw==KW_TRUE||kw==KW_FALSE){
            c=PrecedentTable[a][LEX_ID];
            if(((tExprData*)stackTop(stack))->kw==EXPRESSION)c=0;//nesmi byt 2 termy za sebou
        }
        else c=PrecedentTable[a][b];
        if(c==0)
        {
            b=LEX_EOL;
            c=PrecedentTable[a][b];
        }
//printf("C je:%c\n",c);
        switch(c)
        {
            case '=':
            case '<':
                    if((err=pushExprdata(table,stack,b,NULL))!=ERROR_OK)return err;
                    kw=getToken();
                    break;
            case '>':
                    //najdi pravidla
                    if (stackEmpty(stack))return ERROR_SYNTAX;
                        //E->(E)
                    if((tmpData=((tExprData*)stackPop(stack)))->kw==LEX_R_BRACKET)
                    {
                        itype=NOINSTR;
                        mmuFree(tmpData);

                        if(stackEmpty(stack))return ERROR_SYNTAX;
                        if((tmpData=((tExprData*)stackPop(stack)))->kw!=EXPRESSION)
                            {mmuFree(tmpData);return ERROR_SYNTAX;}
                        dest = tmpData->symbol;
                        mmuFree(tmpData);

                        if(stackEmpty(stack))return ERROR_SYNTAX;
                        if((tmpData=((tExprData*)stackPop(stack)))->kw!=LEX_L_BRACKET)
                            {mmuFree(tmpData);return ERROR_SYNTAX;}
                        mmuFree(tmpData);
                    }
                    //E-> E op E
                    else if(tmpData->kw==EXPRESSION)
                    {
                        sourc2 = tmpData->symbol;
                        mmuFree(tmpData);

                        if(stackEmpty(stack))return ERROR_SYNTAX;
                        tmpData=((tExprData*)stackPop(stack));
                        if(!isOper(tmpData->kw)){mmuFree(tmpData);return ERROR_SYNTAX;}
                        itype = getItype(tmpData->kw);
                        mmuFree(tmpData);

                        if(stackEmpty(stack))return ERROR_SYNTAX;
                        if((tmpData=((tExprData*)stackPop(stack)))->kw!=EXPRESSION)
                            {mmuFree(tmpData);return ERROR_SYNTAX;}
                        sourc1 = tmpData->symbol;
                        mmuFree(tmpData);
                    }
                    else return ERROR_SYNTAX;
                    //vlozit instrukci
                    if(itype!=NOINSTR){//jen v pripade E->EopE
                        dest=insertBlankConstant(table->currentFunc);
                        i=genInstr(itype,dest,sourc1,sourc2);
                        listInsertLast(&(table->currentFunc->instructions),i);
                    }
                    if((err=pushExprdata(table,stack,EXPRESSION,dest))!=ERROR_OK)return err;//pushne se vysledek expr
                    break;

            case '$':

                    tmpData=NULL;
                    if(stackEmpty(stack))return ERROR_SYNTAX;
                    if((tmpData=((tExprData*)stackPop(stack)))->kw!=EXPRESSION)
                        {mmuFree(tmpData);return ERROR_SYNTAX;}
                    *result=tmpData->symbol;
                    break;



            default:/*printf("ERROR - default\n")*/;return ERROR_SYNTAX;
        }


    }while(!(a==LEX_EOL && b==LEX_EOL));

    stackDispose(stack);
    stackDestroy(stack);

    //printf("uspesne expression\n");
    return ERROR_OK;
}

tExprData* newExprdata(tKeyword kw,tSymbol *symbol)
{
    tExprData *new=mmuMalloc(sizeof(tExprData));
    new->kw=kw;
    new->symbol=symbol;
    return new;
}

E_CODE pushExprdata(tSymbolTable *table,tStack *stack,tKeyword kw,tSymbol *symb)
{
    if(kw==LEX_ID)
    {
        if((symb=functionSearchSymbol(table->currentFunc,gToken.data))==NULL)
            return ERROR_SEMANTIC_VARIABLE;
        kw=EXPRESSION; //E->id
    }
    else if (kw==LEX_STRING || kw==LEX_NUMBER ||kw==KW_NIL ||kw==KW_FALSE ||kw==KW_TRUE)
    {
        symb=functionInsertConstant(table->currentFunc,gToken.data,kw);
        kw=EXPRESSION; //E->id
    }
    stackPush(stack,(newExprdata(kw,symb)));
    return ERROR_OK;
}

tKeyword topTerm(tStack *stack)
{
    tStackPtr tmp=stack->top;
    while(tmp!=NULL && (((tExprData*)tmp->data)->kw==EXPRESSION))
          {
              tmp=tmp->ptr;
          }
    return (tmp==NULL) ? EXPRESSION:((tExprData*)tmp->data)->kw;
}

bool isOper(tKeyword kw)
{
    return(kw==LEX_ADDITION||kw==LEX_SUBSTRACTION||kw==LEX_MULTIPLICATION||
           kw==LEX_DIVISION||kw==LEX_POWER||kw==LEX_LESSER||kw==LEX_LESSER_EQUAL||
           kw==LEX_EQUAL||kw==LEX_UNEQUAL||kw==LEX_GREATER||kw==LEX_GREATER_EQUAL);
}

tItype getItype(tKeyword kw)
{
   return (tItype)(kw+(I_ADD-LEX_ADDITION));
}

