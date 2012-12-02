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
    T->lastAdded=NULL;
}

void btFree(tBTree *T){
    deleteNodes(T->root);
    btInit(T);
}

void deleteNodes(tBTNode N){
    if(N->left!=NULL)deleteNodes(N->left);
    if(N->right!=NULL)deleteNodes(N->right);
    mmuFree(N);
}

tBTNode btFind(tBTree *T,tString *key){
    return searchNodes(T->root,key);
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

void AVLBTInsert (tBTree *T, tString *key,void *data){
     T->root=AVLinsertNode(T->root,key,data);
}

tBTNode AVLinsertNode(tBTNode N,tString *key, void *data){
    if (N==NULL){
        N=mmuMalloc(sizeof(struct tBTreeNode));
        N->key=key;
        N->height=0;
        N->left=N->right=NULL;
        N->data=data;
    }
    else
        if(strCmp(key,N->key)<0){
            N->left=AVLinsertNode(N->left,key,data);
            if((Height(N->left) - Height(N->right)) ==2){
                if(strCmp(key,N->left->key)<0)
                    N=singleRotateLeft(N);
                else
                    N=doubleRotateLeft(N);
            }
         }
         else
             if(strCmp(key,N->key)>0){
                 N->right=AVLinsertNode(N->right,key,data);
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
