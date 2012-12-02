#include"list.h"

void initList(tList *L){
    L->first=L->last=NULL;
}

E_CODE listInsertLast(tList *L,void *data){
    tListItem *tmp=mmuMalloc(sizeof(tListItem));
    if (tmp==NULL) return ERROR_COMPILATOR;
    tmp->data=data;
    tmp->next=NULL;
    if(L->first==NULL) L->first=L->last=tmp;
    else L->last->next=tmp;
    L->last=tmp;
    return ERROR_OK;
}

E_CODE listInsertFirst(tList *L, void *data){
    tListItem *tmp=mmuMalloc(sizeof(tListItem));
    if (tmp==NULL) return ERROR_COMPILATOR;
    tmp->data=data;
    tmp->next=L->first;
    if(L->first==NULL) L->last=tmp;
    L->first=tmp;
    return ERROR_OK;
}

E_CODE listDeleteFirst(tList *L){
    if(L->first==NULL) return ERROR_OK;
    if(L->first==L->last)L->last=NULL;
    tListItem *tmp=L->first;
    L->first=L->first->next;
    mmuFree(tmp);
    return ERROR_OK;
}
