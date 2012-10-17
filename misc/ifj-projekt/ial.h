/*
 * @description   Povinny soubor do IAL - tento soubor neni primou soucasti implementace
 *                Funkce jsou zkopirovane ze souboru: binary_tree.* a library.*
 *
 * @author        Vendula Poncova - xponco00    Vyhledavani v retezci
 *                Patrik Hronský  - xhrons00    Razeni
 *                Marek Salat     - xsalat00    Tabulka symbolu
 * @projekt       IFJ11
 * @date
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////////// VYHLEDÁVÁNÍ V ØETÌZCI :

int kmp        (char *text, char *vzor);
void kmpVektor (char *vzor, int vektor[]);
int kmpHledej  (char *text, char *vzor, int vektor[]);

/////////////////////////////////////////////////////////////////////////////////// ØAZENÍ :

void mergeSort  (char *str);
void copyArray  (char *dest, char *src, int strLen);
void copyToSrc  (int strLen, char *arr);
bool isSorted   (int strLen, char *arr);
void copyRight  (int *k, int *j, int *copied, char *arr, int step);
void copyLeft   (int *k, int *i, int *copied, char *arr, int step);

/////////////////////////////////////////////////////////////////////////////////// TABULKA SYMBOLÙ :

#define INS_OK          1  // vlozeno v poradku
#define INS_NODE_EXIST  0  // prvek se zadanym klicem uz existuje
#define INS_MALLOC     -5  // chyba pri alokaci
#define INS_TREE_NULL  -5  // misto stromu NULL
#define INS_KEY_NULL   -5  // misto klice NULL

typedef enum {
   DEFAULT,       // data budou void repektive zadna, nijak se nemazou
   FUNCIONS,      // data se pretypuji na TFunction*
   VAR,           // tady jeste nevim 28.10.2011 jak bude vypadat polozka pro symbol|identifikator
} EBTreeDataType;

typedef struct TBTreeNode {
   struct TBTreeNode *left;   // levy podstrom
   struct TBTreeNode *right;  // pravy podstrom
   char  *key;                // vyhledavaci klic
   int    height;             // vyska nejdelsi vetve podstromu
   void  *data;               // data uzivatele, predem nevim jaka, data si prida uzivatel
} *TNode;

typedef struct {
   TNode root;                // koren stromu
   TNode lastAdded;           // ukazatel na posledni pridanou polozku(nekdy se to muze hodit)
   int   nodeCount;           // pocet uzlu
   EBTreeDataType type;       // podle typu stromu poznam jak TNode->data smazat
} TBTree;


void  BTreeInit  (TBTree*, EBTreeDataType);
int   BTreeInsert(TBTree*, char*, void*);
void  BTreeDelete(TBTree*);
TNode BTreeSearch(TBTree*, char*);

int   insert     (TNode*, char*, void*);
void  deleteNode (TNode);
TNode search     (TNode, char*);

/* konec ial.h */
