#ifndef BINARY_TREE_H_INCLUDED
#define BINARY_TREE_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include "strings.h"
#include "mmu.h"
#include "errors.h"

typedef struct tBTreeNode {
    struct tBTreeNode *right;
    struct tBTreeNode *left;
    void *data;
    tString *key;
    int height;
  } *tBTNode;


typedef struct{
    tBTNode root;
    tBTNode lastAdded;
    //nevim co jeste sem dat
}tBTree;

int Max( int, int);
void btInit(tBTree*);
void btFree(tBTree*);
void deleteNodes(tBTNode);
tBTNode btFind(tBTree*,tString*);
tBTNode searchNodes(tBTNode, tString*);
tBTNode singleRotateLeft(tBTNode);
tBTNode singleRotateRight(tBTNode);
tBTNode doubleRotateLeft(tBTNode);
tBTNode doubleRotateRight(tBTNode);
E_CODE BTInsert (tBTree*, tString*,void*);
tBTNode AVLinsertNode(tBTNode,tString*, void*);
void AVLBTInsert(tBTree*, tString*,void*);
#endif
