#include"list.h"

void initList(tList *L){
    L->first=NULL;
    L->last=NULL;
    L->act=NULL;
}

E_CODE listInsertLast(tList *L,void *data){
    tListItem *tmp=mmuMalloc(sizeof(tListItem));
    if (tmp==NULL) return ERROR_COMPILATOR;
    tmp->data=data;
    tmp->next=NULL;
    if(L->first==NULL){ L->first=L->last=tmp;}
    else {L->last->next=tmp;}
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
void Last (tList *L)		{
	L->act=L->last;

}
void postInsert (tList *L, void *data)	{

    tListItem *new; 
    new=mmuMalloc(sizeof(tListItem));
    new->data=data;
	if (L->act!=NULL)//seznam je aktivni
          {
            new->next=L->act->next; //ukazatel nového prvku naplní ukazatelem z aktuálního 
            L->act->next=new; //vloží nový prvek za aktuální
            if(new->next==NULL) L->last=new;
          }
    else if(L->first==NULL)//seznam je prazdny  - vlozime 1.prvek
         {
            new->next=NULL;
            L->first=L->last=new; 
         }
    else//seznam je neaktivni, vlozime 1.prvek
        {
           new->next=L->first;
           L->first=new;
        }

}
void Succ (tList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if (L->act!=NULL) //kontrola aktivity
            L->act=L->act->next; //posun aktivního prvku
}
