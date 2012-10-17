/*
 * @description   Prace s binarnim vyheldavacim stromem 16.11.2011 uz neni AVL :(
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

#include "binary_tree.h"
/*
 * vyhleda misto kde polozka patri a vlozi ji
 * @param   uzel
 * @param   klic podle ktereho hledat
 * @param   ukazatel na data(jedno jaka)
 * @return  vraci ukazatel na uzel(muze totiz nekdy dojit k tomu ze je treba strom vyvazit
 *          proto se musi zasadne volat jako (TNode)root=insert(root, "key", NULL)
 * navic funkce nastavuje urcite pomocne promene(position-pozice posledniho vkladaneho prvku, returnValue-makra INS_XXX)
 */
int insert(TNode*, char*, void*);

/*
 * maze uzly stromu, pomocna funkce pro BTreeDelet
 * smaze i data pode predpisu ktery je v tele funkce
 * @param uzel stromu
 * @param jak mazat data
 */
void deleteNode(TNode);

/*
 * hleda uzel, pomocna promena BTreeSearch
 * @param   uzel
 * @param   klic
 * @return  pozice prvku
 */
TNode search(TNode, char*);


//----------------------------------------------------------------------
//----------------------------------------------------------------------

void BTreeInit(TBTree *T, EBTreeDataType type) {
   if(T == NULL)
      return;
   T->root = NULL;
   T->lastAdded = NULL;
   T->nodeCount = 0;
   T->type = type;
}
//----------------------------------------------------------------------

void BTreeDelete(TBTree *T) {
   if(T == NULL)
      return;
   deleteNode(T->root);
   BTreeInit(T, T->type);
}

void deleteNode(TNode n) {
   if(n != NULL) {
      deleteNode(n->left);
      deleteNode(n->right);
      free(n);
   }
}

//----------------------------------------------------------------------

TNode BTreeSearch(TBTree *T, char *key) {
   if(T == NULL || key == NULL)
      return NULL;
   return search(T->root, key);
}

TNode search(TNode T, char *key) {
   TNode   tmp   = T;

   while( tmp != NULL) {
      int cmpResult = strcmp(key, tmp->key);

      if( cmpResult < 0)
         tmp = tmp->left;
      else if( cmpResult > 0)
         tmp = tmp->right;
      else
         break;
   }
   return tmp;
}

//----------------------------------------------------------------------

// slouzi jako pomocna promena funkce BTreeInsert,
// abych po vlozeni prvku mohl nastavi T->lastAdded
TNode position = NULL;

int BTreeInsert(TBTree *T, char *key, void *data) {
   if(T == NULL)
      return INS_TREE_NULL;
   if(key == NULL)
      return INS_KEY_NULL;

   int err = insert( &( T->root), key, data);

   if(err != INS_OK)  // insert meni returnValue proto jej kontroluju
      return err;

   T->lastAdded = position;
   T->nodeCount++;
   return INS_OK;
}

int insert(TNode *T, char *key, void *data) {
   TNode   *tmp  = T;

   while( *tmp != NULL) {
      (*tmp)->height++;
      int cmpResult = strcmp(key, (*tmp)->key);

      if( cmpResult < 0)
         tmp = &( (*tmp)->left );
      else if( cmpResult > 0)
         tmp = &( (*tmp)->right );
      else
         return INS_NODE_EXIST;
   }

   *tmp = malloc( sizeof(struct TBTreeNode) );
   if(*tmp == NULL)
      return INS_MALLOC;

   // inicializace uzlu
   position       = *tmp;                 // globalni promena
   (*tmp)->left   = NULL;
   (*tmp)->right  = NULL;
   (*tmp)->height = 0;
   (*tmp)->data   = data;
   (*tmp)->key    = key;

   return INS_OK;
}
