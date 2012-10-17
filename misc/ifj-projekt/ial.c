/*
 * @description   Povinny soubor do IAL - tento soubor neni primou soucasti implementace
 *                Funkce jsou zkopirovane ze souboru: binary_tree.* a library.*
 *
 * @author        Vendula Poncova - xponco00    Vyhledavani v retezci
 *                Patrik Hronsk�  - xhrons00    Razeni
 *                Marek Salat     - xsalat00    Tabulka symbolu
 * @projekt       IFJ11
 * @date
 */

#include "ial.h"

/////////////////////////////////////////////////////////////////////////////////// VYHLED�V�N� V �ET�ZCI :

/*
 * Funkce pro nalezen� pod�et�zce v �et�zci.
 * Nejsp� se z n� stane funkce sort(string,string).
 * @param   prohled�van� �et�zec
 * @param   hledan� �et�zec
 * @return  pozice prvn�ho v�skytu vzoru v textu
 *          pokud vzor nen� nalezen, vr�t� se -1
 */
int kmp (char *text, char *vzor) {

  // vzor je pr�zdn� �et�zec, vra� nulu
  if (strlen(vzor) == 0) return 0;
  
  int dVzoru = strlen(vzor);
  int vektor[dVzoru];

  // nalezen� fail vektoru pro vzor
  kmpVektor(vzor, vektor);

  // vra� index vzoru v textu
  return kmpHledej(text, vzor, vektor);
}

/*
 * Pro dan� vzor najde p�echody mezi stavy KMP automatu 
 * a zap�e je do vektoru FAIL.
 * @param   vzorov� �et�zec
 * @param   pole integer� - vektor
 */
void kmpVektor (char *vzor, int vektor[]) {

  int delka = strlen(vzor);  // d�lka �et�zce vzor
  int stav;                  // aktu�ln� pozice ve vzoru (index)
  int prechod;               // n�vratov� index pro dan� stav (hodnota v poli vektor)

  vektor[0] = -1;            // z nult�ho stavu n�vrat do po��te�n�ho stavu "-1" 
                             // stav "-1" existuje pouze teoreticky

  for (stav = 1; stav < delka; stav++) { // pro ka�dou pozici ve vzoru:

    prechod = vektor[stav-1];            // ulo� p�echod p�edchoz�ho stavu

    while ( (prechod > -1) && (vzor[prechod] != vzor[stav-1]) ) {

      /* P�echod nevede do po��te�n�ho stavu a znak ve stavu, do kter�ho
         se vrac�me, je r�zn� od znaku na p�edchoz� pozici.
         Provedeme n�vrat pomoc� p�echodu na ni��� pozici
         a ulo��me si nov� p�echod.
      */
      prechod = vektor[prechod];
    }
    
    /* P�echod vede do po��te�n�ho stavu (nen� tedy kam se d�l posunovat)
       nebo porovn�van� znaky se rovnaly.
       P�i��t�me 1, nebo� nalezen� "suffix", na kter� se lze p�esunout
       roz�i�ujeme o nov� znak.
    */
    vektor[stav] = prechod + 1;
  }
}

/*
 * Vyhled�v� pod�et�zec vzor v �et�zci text pomoc�
 * KMP algoritmu. Pozice se ��sluje od 1.
 * @param   prohled�van� �et�zec
 * @param   hledan� �et�zec
 * @param   vektor FAIL
 * @return  pozice prvn�ho v�skytu vzoru v textu
 *          pokud vzor nen� nalezen, vr�t� se -1
 */
int kmpHledej (char *text, char *vzor, int vektor[]) {

  int i = 0;                  // index v textu
  int stav = 0;               // stav vzoru
  int maxStav = strlen(vzor); // koncov� stav

  while (text[i] != '\0' && stav < maxStav) {
    // nedo�etli jsme text do konce 
    // a nedo�li do koncov�ho stavu 

    if (stav == -1 || text[i] == vzor[stav]) {
      // p�esun na nov� znak textu 
      // a do n�sleduj�c�ho stavu vzoru
      i++;
      stav++;
    }
    else {
      // z�st�v�me na aktu�ln�m znaku textu
      // n�vrat do p�edchoz�ho stavu dan�ho vektorem
      stav = vektor[stav];
    }

  } // while

  if (stav == maxStav) {
  // vr�t� index, na jak�m byl vzor nalezen
    return (i - maxStav + 1);
  }
  // vzor nebyl nenalezen
  else return -1; 

}

/////////////////////////////////////////////////////////////////////////////////// �AZEN� :


/*
 * Funkce se�ad� �et�zec (pole znak�) podle ordin�ln�ch hodnot.
 * Pou�it� agoritmus: merge sort
 * @param   �et�zec
 */
void mergeSort(char *str) {
   if(str == NULL)  return;

   // inicializace:
   int strLen = strlen(str);  // delka retezce
   if (strLen == 0) return;

   char arr[2*strLen];        // pomocne pole o dva krat velikosti retezce bez \0
   copyArray(arr,str,strLen); // skopiruju retezec bez \0
                              // do leve casti pomocneho pole

   int copied = 0;            // pocet prvku ktere jiz byly skopirovany do ciloveho pole
   int cntRise = 2;           // pocitadlo posloupnosti
   bool endLeft,endRight,endMiddle,sourceLeft;
   int i,j,k,l,step,extra;

   // t��d�n�:
   sourceLeft=true;           // pocatecni smer postupu - leve pole je zdroj, prave cil

  while(cntRise>1) {
  // dokud neni posloupnost serazena:

    cntRise=0;
    copied=0;
    if(sourceLeft) {         // jestli smer postupu je z leve casti pole do prave
      i=0;                   // zacatek pole
      j=strLen-1;            // konec leve casti pole
      k=strLen;              // zacatek prave casti pole
      l=2*strLen-1;          // konec pole
    }
    else {                   // jestli smer postupu je z prave casti pole do leve
      k=0;
      l=strLen-1;
      i=strLen;
      j=2*strLen-1;
    }
    step=1;  // krok - smer postupu cilovym polem - zleva doprava o jedno

    while(i!=j || copied!=strLen) {  
      // dokud se i a j nesetkaji a dokud nebyly
      // skopirovany vsechny prvky zdrojoveho pole

      endLeft=false;         // konec neklesajici posloupnosti zleva
      endRight=false;        // konec neklesajici posloupnosti zprava
      endMiddle=false;       // setkani se i a j

      while(!endLeft && !endRight && !endMiddle) {  
        // dokud neskoncila
        // neklesajici posloupnost zleva, zprava,
        // nebo dokud se i a j nesetkaji

        if(arr[i]<arr[j]) {  // jestli prvek v leve casti zdrojoveho pole
                             // je mensi nez prvek v prave casti
          arr[k]=arr[i];     // vloz prvek do ciloveho pole
          copied++;
          if(i==j) {         // jestli se i a j setkali
            endMiddle=true;
          }
          else {             // jestli se i a j jeste nesetkali
            i++;             // posun i
            k+=step;         // posun k (index ciloveho pole)
            if(arr[i]<arr[i-1]) {
              endLeft=true;  // jestli skoncila neklesajici posloupnost zleva
	          cntRise++;
            }
          }
        }

        else {  
          // jestli prvek v prave casti zdrojoveho pole je mensi nez prvek v leve casti

          arr[k]=arr[j];     // vloz prvek do ciloveho pole
          copied++;
          if(i==j) {         // jestli se i a j setkali
            endMiddle=true;
          }
          else {             // jestli se i a j jeste nesetkali
            j--;             // posun j
            k+=step;         // posun k (index ciloveho pole)
            if(arr[j]<arr[j+1]) {
              // jestli skoncila neklesajici posloupnost zprava
              endRight=true;
	          cntRise++;
            }
          }
        }
      }

      if(endLeft) {          // jestli skoncila neklesajici posloupnost zleva
        copyRight(&k,&j,&copied,arr,step);  // skopiruj pravou neklesajici
        endLeft=false;
	      cntRise++;
      }
      if(endRight) {         // jestli skoncila neklesajici posloupnost zprava
        copyLeft(&k,&i,&copied,arr,step);  // skopiruj levou neklesajici
        endRight=false;
	      cntRise++;
      }
      extra=k;               // prehozeni k a l
      k=l;
      l=extra;
      step=-step;            // obraceni polarity postupu cilovym polem
    }

    sourceLeft=!sourceLeft;  // zmena smeru postupu (prehozeni zdrojoveho a ciloveho pole)

    if(!sourceLeft) {        // jestli byla leva cast pole zdrojova
      copyToSrc(strLen,arr); // skopiruju cilove pole do zdrojoveho
    }

  } // while, dokud pole nen� set��zen�

  copyArray(str,arr,strLen); // skopiruju jiz serazenou posloupnost znaku
                             // z pomocneho pole na adresu puvodniho retezce
}

// skopiruju retezec do pomocneho pole nebo naopak
void copyArray(char *dest, char *src, int strLen) {
  for(int x=0;x<strLen;x++)
    dest[x]=src[x];
}

// skopiruju cilove pole do zdrojoveho
void copyToSrc(int strLen, char *arr) {
  for(int x=0;x<strLen;x++)
    arr[x]=arr[x+strLen];
}

/*
 * skopiruje zbytek neklesajici posloupnosti zprava do ciloveho pole
 */
void copyRight(int *k, int *j, int *copied, char *arr, int step) {
  bool endLeft=true;
  while(endLeft) {
    arr[(*k)]=arr[(*j)];
    (*copied)++;
    (*j)--;
    (*k)+=step;
    endLeft=(arr[(*j)]>=arr[(*j)+1]);  // jestli skoncila neklesajici posloupnost
  }                                    // zprava, endLeft bude false a ukonci cyklus
}

/*
 * skopiruje zbytek neklesajici posloupnosti zleva do ciloveho pole
 */
void copyLeft(int *k, int *i, int *copied, char *arr, int step) {
  bool endRight=true;
  while(endRight) {
    arr[(*k)]=arr[(*i)];
    (*copied)++;
    (*i)++;
    (*k)+=step;
    endRight=(arr[(*i)]>=arr[(*i)-1]);  // jestli skoncila neklesajici posloupnost
  }                                     // zleva, endRight bude false a ukonci cyklus
}


/////////////////////////////////////////////////////////////////////////////////// TABULKA SYMBOL� :

/*
 *    inicializace stromu
 *    @param   strom
 *    @param   typ stromu
 */
void BTreeInit(TBTree *T, EBTreeDataType type) {
   if(T == NULL)
      return;
   T->root = NULL;
   T->lastAdded = NULL;
   T->nodeCount = 0;
   T->type = type;
}
//----------------------------------------------------------------------

/*
 *    smaze cely strom
 *    @param   ukazatel na strom
 */
void BTreeDelete(TBTree *T) {
   if(T == NULL)
      return;
   deleteNode(T->root);
   BTreeInit(T, T->type);
}

/*
 * maze uzly stromu, pomocna funkce pro BTreeDelet
 * smaze i data pode predpisu ktery je v tele funkce
 * @param uzel stromu
 * @param jak mazat data
 */
void deleteNode(TNode n) {
   if(n != NULL) {
      deleteNode(n->left);
      deleteNode(n->right);
      free(n);
   }
}

//----------------------------------------------------------------------

/*
 *    vyhleda ve stromu podle klice
 *    @param   ukazatel na strom
 *    @param   klic hledaneho uzlu
 *    @return  pozice uzlu, pokud uzel nebyl nalezen vraci NULL
 */
TNode BTreeSearch(TBTree *T, char *key) {
   if(T == NULL || key == NULL)
      return NULL;
   return search(T->root, key);
}

/*
 * hleda uzel, pomocna promena BTreeSearch
 * @param   uzel
 * @param   klic
 * @return  pozice prvku
 */
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

/*
 * vyhleda misto kde polozka patri a vlozi ji
 * @param   uzel
 * @param   klic podle ktereho hledat
 * @param   ukazatel na data(jedno jaka)
 * @return  vraci ukazatel na uzel(muze totiz nekdy dojit k tomu ze je treba strom vyvazit
 *          proto se musi zasadne volat jako (TNode)root=insert(root, "key", NULL)
 * navic funkce nastavuje urcite pomocne promene(position-pozice posledniho vkladaneho prvku, returnValue-makra INS_XXX)
 */
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

/* konec ial.c */
