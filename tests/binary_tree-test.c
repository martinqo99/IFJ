#include <stdio.h>
#include "../src/binary_tree.h"

int main()
{
    tBTree *T;
    tBTNode N;
    
 
    T=mmuMalloc(sizeof(tBTree));
    btInit(T);
    tString str,str1,str2;
    void *nic=NULL;
    str=strCreate("ahoj");
    str1=strCreate("nazdar");
    str2=strCreate("valhala");
    //printf("%s\n",str.data);
    BTInsert(T,&str,nic);
    BTInsert(T,&str1,nic);
    BTInsert(T,&str2,nic);
    printf("%d\n",T->root->height);
    N=btFind(T,&str1);
    //BTInsert(T,&str2,nic);
    if(N!=NULL)printf("%s\n",N->key->data);
    else printf("nic se nenaslo\n");
    if(T->root->left==NULL && T->root->right==NULL) printf("chyba\n");
    int cmp=strCmp(&str1,&str);
    printf("%d\n",cmp);
    btFree(T);
    return 0;
}
