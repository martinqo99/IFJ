/*
 * @description   Prace s binarnim vyheldavacim stromem
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

#ifndef BINARYTREEAVL_H_INCLUDED
#define BINARYTREEAVL_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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


//--------------------------------------------------------------------------------
/*
 *    inicializace stromu
 *    @param   strom
 *    @param   typ stromu
 */
void BTreeInit(TBTree*, EBTreeDataType);
//--------------------------------------------------------------------------------


/*
 *    funkce prida polozku do stromu
 *    konevce - data pridava uzivatel
 *    @param   ukazatel na strom
 *    @param   ukazatel na retezec
 *    @param   ukazatel na data(jedno jaka)
 *    @return  INS_OK         pri uspesnem volozeni,
 *             INS_NODE_EXIST      pri nevlozeni(polozka se stejnym klicem jiz ve stromu existuje),
 *             INS_MALLOC     pri nepovedene alokaci
 *             INS_TREE_NULL misto stromu NULL
 *             INS_KEY_NULL  misto klice NULL
 *             T->lastAdded se ulozi pozice posledni PRIDANE polozky
 */
int BTreeInsert(TBTree*, char*, void*);
//--------------------------------------------------------------------------------

/*
 *    smaze cely strom
 *    @param   ukazatel na strom
 */
void BTreeDelete(TBTree*);
//--------------------------------------------------------------------------------

/*
 *    vyhleda ve stromu podle klice
 *    @param   ukazatel na strom
 *    @param   klic hledaneho uzlu
 *    @return  pozice uzlu, pokud uzel nebyl nalezen vraci NULL
 */
TNode BTreeSearch(TBTree*, char*);

#endif // BINARYTREEAVL_H_INCLUDED
