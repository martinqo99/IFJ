#include "binary_tree.h"

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
}

void btFree(tBTree *T){
    deleteNodes(T->root);
    btInit(T);
}

void deleteNodes(tBTNode N){
    if(N->left!=NULL)deleteNodes(N->left);
    if(N->right!=NULL)deleteNodes(N->right);
    free(N);
}

tBTNode btFind(tBTree *T,tString *key){
    return searchNodes(T->root,key);
}

tBTNode searchNodes(tBTNode N, tString *key){
    if(N==NULL) return NULL;
    else {
         int cmp=strCmp(N->key,key);
         if(cmp!=0){
             if(cmp<0)return searchNodes(N->left,key);
             else return searchNodes(N->right,key);
         }
         else return N;
    } 
}

tBTNode singleRotateLeft(tBTNode N2){
    tBTNode N;
    N=N2->left;
    N->right=N2;
  
    N2->height=Max(Height(N2->left),Height(N2->right))+1;
    N->height=Max(Height(N->left),N2->height)+1;

    return N;
}

tBTNode singleRotateRight(tBTNode N){
    tBTNode N2;
    N2=N->right;
    N->right=N2->left;
    N2->left=N;
  
    N->height=Max(Height(N->left),Height(N->right))+1;
    N2->height=Max(Height(N2->right),N->height)+1;

    return N2;
}

tBTNode doubleRotateLeft(tBTNode N){
    N->left=singleRotateRight(N->left);
    return singleRotateLeft(N);
}

tBTNode doubleRotateRight(tBTNode N){
    N->right=singleRotateLeft(N->right);
    return singleRotateRight(N);
}

void BTInsert (tBTree *T, tString *key,void *data){
     T->root=insertNode(T->root,key,data);
}

tBTNode insertNode(tBTNode N,tString *key, void *data){
    if (N==NULL){
        N=mmuMalloc(sizeof(struct tBTreeNode));
        N->key=key;
        N->height=0;
        N->left=N->right=NULL;
        N->data=data;
    }
    else
        if(strCmp(key,N->key)<0){
            N->left=insertNode(N->left,key,data);
            if((Height(N->left) - Height(N->right)) ==2){
                if(strCmp(key,N->left->key)<0)
                    N=singleRotateLeft(N);
                else
                    N=doubleRotateLeft(N);
            }
         }
         else
             if(strCmp(key,N->key)>0){
                 N->right=insertNode(N->right,key,data);
                 if((Height(N->right) - Height(N->left)) ==2){
                     if(strCmp(key,N->right->key)>0)
                         N=singleRotateRight(N);
                     else
                         N=doubleRotateRight(N);
                 }
             }

             N->height=Max(Height(N->left),Height(N->right))+1;
             return N;
}
