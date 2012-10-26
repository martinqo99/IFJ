/**
 * Jednosmerne vazany linearni seznam.
 * Struktura tList obsahuje polozku tListItem
 * Autor: Veronika Necasova (xnecas21)
 * Vytvoreno: 23. 10. 2012
 * Upraveno: 24. 10. 2012
 * Verze: 0.1
 **/
 
 #include<stdio.h>
 #include<stdlib.h>
 #include<stdarg.h>
 
 /**
  * Struktura pro polozku seznamu
  @*next ... ukazatel na nasledujici polozku seznamu
  @data .... promenna, v niz jsou ulozena data (typ int)
 **/
 typedef struct tItem {
	 struct tItem *next;
	 int data;
 } *tListItem;
 
 
/**
 * Struktura reprezentuje jednosmerne vazany linearni seznam
 @active ... ukazatel na aktivni polozku seznamu
 @first .... ukazatel na prvni polozku seznamu
*/
 typedef struct {
	 tListItem active;
	 tListItem first;
 } tList;


//definice fuknci 
void initList (tList *);
void disposeList (tList *);
void insertFirst (tList *, int);
void first (tList *);
void deleteFirst (tList *);
void postDelete (tList *);
void postInsert (tList *, int);
void succ (tList *);
void copy (tList *, int *);	
void actualize (tList *, int );
int  active (tList

/**
 * Inializace jednosmerne vazaneho linearniho seznamu
 @*list ... ukazatel na strukturu linearne vazaneho seznamu
**/
void initList (tList *list) {
	list->active = NULL;
	list->first = NULL;
}

/**
 * Vlozi polozku na zacatek seznamu
 @*list ... ukazatel na strukturu linearne vazaneho seznamu
 @value ... hodnota nove polozky
**/
void insertFirst (tList *list, int value) {
	struct tListItem newItem;
	//alokace mista pro novou polozku
	if ((newItem = malloc(sizeof(struct tListItem))) == NULL) {
		//chybna alokace
		Error();
		//ukonceni provadeni funkce
		return;
	//naplneni daty
	newItem->data = value;
	newItem->next = list->first;
	//vlozeni na zacatek seznamu
	list->first = newItem;
}
