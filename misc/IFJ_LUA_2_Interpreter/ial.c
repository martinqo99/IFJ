/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   ial.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */


#include "garbage_collector.h"
#include "funkcie.h"
#include "ial.h"
#include "vest_funkce.h"
#include "vyrazy.h"


tBTSUzolPtr ts;


static void BTSinit(tBTSUzolPtr *koren);
tBTSUzolPtr BTSsearch(tBTSUzolPtr koren, tNazov kluc);
static tBTSUzolPtr BTSinsert(tBTSUzolPtr *koren, tNazov kluc, tSymbol data);
static void BTSdispose(tBTSUzolPtr *koren);
static void QSswap(char *prvy, char *druhy);
static void QSpartition(char *string, int *i, int *j);
static void QSsort(char *string, int left, int right);
static void BMAcomputeCharJump(char *pattern, int patlen, int *charJump);
static void BMAcomputeMatchJump(char *pattern, int patlen, int *matchJump);
static int BMAsearch(char *string, char *pattern);


void TSinit(void)
{
    BTSinit(&ts);
}


void TSdispose(void)
{
    BTSdispose(&ts);
}


tSymbol *TSvlozSymbol(tSymbol symbol)
{
    tBTSUzolPtr uzol;
    uzol = BTSinsert(&ts, symbol.nazov, symbol);

    if (uzol)
        return &(uzol->data);

    return NULL;
}

tSymbol *TSvlozBool(tNazov nazov, bool data)
{
    tSymbol symbol;

    symbol.nazov = nazov;
    symbol.typ = tBool;
    symbol.value.b = data;

    return TSvlozSymbol(symbol);
}


tSymbol *TSvlozDouble(tNazov nazov, double data)
{
    tSymbol symbol;

    symbol.nazov = nazov;
    symbol.typ = tDouble;
    symbol.value.d = data;

    return TSvlozSymbol(symbol);
}

tSymbol *TSvlozString(tNazov nazov, char *string)
{
    tSymbol symbol;

    symbol.nazov = nazov;
    symbol.typ = tString;
    symbol.value.s = string;

    return TSvlozSymbol(symbol);
}

void TSinitSymbol(tSymbol *symbol)
{
    if (symbol != NULL)
    {
        symbol->nazov = NULL;
        symbol->typ = tNil;
    }
}


tBTSUzolPtr TSreadSymbol(tNazov nazov)
{
    tBTSUzolPtr uzol;

    if (ts != NULL)
    {
        uzol = BTSsearch(ts, nazov);

        if (uzol != NULL)
            return uzol;
    }

    return NULL;
}

static void BTSinit(tBTSUzolPtr *koren)
{
    *koren = NULL;
}


tBTSUzolPtr BTSsearch(tBTSUzolPtr koren, tNazov kluc)
{
    int i;

    // ak existuje koren
    if (koren != NULL)
    {
        i = strcmp(koren->data.nazov, kluc);

        // ak sa kluce rovnaju
        if (i == 0)
        {
            return koren;
        }
        // ak je hladany kluc mensi vnorime sa do laveho podstromu
        else if (i < 0)
        {
            return BTSsearch(koren->lptr, kluc);
        }
        // ak je hladany kluc vecsi vnorime sa do praveho podstromu
        else
        {
            return BTSsearch(koren->rptr, kluc);
        }
    }

    return NULL;
}

static tBTSUzolPtr BTSinsert(tBTSUzolPtr *koren, tNazov kluc, tSymbol symbol)
{
    int i;

    // ak koren neexistuje
    if (*koren == NULL)
    {
        // alokujeme strukturu
        if (!(*koren = advMalloc(sizeof(struct tBTSUzol))))
        {
            error = EINTERN;
            return NULL;
        }

        // naplnime strukturu
        (*koren)->kluc = kluc;
        (*koren)->data = symbol;
        (*koren)->lptr = NULL;
        (*koren)->rptr = NULL;
    }
    // ak koren existuje
    else
    {
        i = strcmp((*koren)->kluc, kluc);

        // ak je vkladany kluc mensi nez kluc uzlu
        if (i < 0)
        {
            return BTSinsert(&((*koren)->lptr), kluc, symbol);
        }
        // ak je vkladany kluc vecsi nez kluc uzlu
        else if (i > 0)
        {
            return BTSinsert(&((*koren)->rptr), kluc, symbol);
        }
        // ak sa uzly rovnaju prepiseme hodnotu uzlu
        else
        {
            (*koren)->data = symbol;
        }
    }

    return *koren;
}


static void BTSdispose(tBTSUzolPtr *koren)
{
    // ak uzol existuje
    if (*koren != NULL)
    {
        // uvolnime lavy podstrom
        BTSdispose(&((*koren)->lptr));

        // uvolnime pravy podstrom
        BTSdispose(&((*koren)->rptr));

        // uvolnime uzol
        /* //free((*koren)->data.nazov);
         if((*koren)->data.typ == tString)
             //free((*koren)->data.value.s);
         */
        //free(*koren);
        *koren = NULL;
    }
}

static void QSswap(char *prvy, char *druhy)
{
    // ak nejde o ten isty prvok
    if (prvy != druhy)
    {
        // xor swap
        *prvy ^= *druhy;
        *druhy ^= *prvy;
        *prvy ^= *druhy;
    }
}


static void QSpartition(char *string, int *i, int *j)
{
    // pseudomedian
    int pm = (int) string[(*i + *j) / 2];

    // kym je lavy mensi nez pravy
    do
    {
        // ak je hodnota mensia nez pseudomedian
        while ((int) string[*i] < pm)
            (*i)++;

        // ak je hodnota mensia nez pseudomedian
        while ((int) string[*j] > pm)
            (*j)--;

        // ak su intervali nie su disjunktne
        if (*i <= *j)
        {
            QSswap(&string[*i], &string[*j]);
            (*i)++;
            (*j)--;
        }
    }
    while (*i < *j);
}


static void QSsort(char *string, int left, int right)
{
    // pomocne premmene na rozdelenie pola
    int i, j;

    // inicializujeme
    i = left;
    j = right;

    // rozdelime pole
    QSpartition(string, &i, &j);

    // radime lavu polku
    if (left < j)
        QSsort(string, left, j);

    // radime pravu polku
    if (i < right)
        QSsort(string, i, right);
}

void ialSort(char *string)
{
    // najpravsi index retazca
    int rind = strlen(string) - 1;

    QSsort(string, 0, rind);
}

static void BMAcomputeCharJump(char *pattern, int patlen, int *charJump)
{
    // pomocna premenna
    int i;

    // inicializujeme
    for (i = 0; i < 256; i++)
        charJump[i] = patlen;

    // naplnime pole podla vzorku
    for (i = 0; i < patlen; i++)
        charJump[(int) pattern[i]] = patlen - (i + 1);
}

static void BMAcomputeMatchJump(char *pattern, int patlen, int *matchJump)
{
    int k,q;
    int backup[patlen+1];

    for (k = 0; k < patlen+1; k++)
    {
        backup[k] = 0;
        matchJump[k] = 0;
    }

    k = patlen;
    q = k + 1;

    backup[k] = q;


    while (k > 0)
    {
        while ((q <= patlen) && (pattern[q-1] != pattern[k-1]))
        {
            if (matchJump[q] == 0)
                matchJump[q] = q - k;

            q = backup[q];
        }

        k--;
        q--;

        backup[k] = q;
    }

    q = backup[0];

    for (k = 0; k <= patlen; k++)
    {
        if (matchJump[k] == 0)
            matchJump[k] = q;

        if (k == q)
            q = backup[q];
    }
}


static int BMAsearch(char *string, char *pattern)
{
    // deklarujeme a inicializujeme dlzky retazcov, pomocne premenne a polia
    int strl = strlen(string);
    int patlen = strlen(pattern);
    int charJump[256];
    int matchJump[patlen + 1];
    int str = 0;
    int pat = patlen - 1;

    // vypocitame skoky
    BMAcomputeCharJump(pattern, patlen, charJump);
    BMAcomputeMatchJump(pattern, patlen, matchJump);

    // ak je retazec alebo vzorka prazdna, alebo je retazec kratsi ako vzorka
    if (patlen == 0)
        return 0;
    if ((strl <= 0) || (strl < patlen))
        return -1;
    else
    {
        while (str <= strl - patlen)
        {
            while (string[str + pat] == pattern[pat])
            {
                if (pat == 0)
                    return str + 1;
                pat--;
            }

            str += ((charJump[(int) string[str + pat - 1]] > matchJump[pat + 1]) ? charJump[(int) string[str + pat - 1]] : matchJump[pat + 1]);

            pat = patlen - 1;
        }
    }

    // ak sme nic nenasli
    return -1;
}

int ialSearch(char *string, char *pattern)
{
    return BMAsearch(string, pattern);
}


/*
 * Funkce radi polozky podle velikosti
 */
void quickSort(char* stringToSort, int left, int right)
{
    // na zaklade skript IAL
    char pom;

    int i = left;
    int j = right;

    // urceni pivotu jako prostredni hodnota
    int pivot = stringToSort[(i+j)/2];

    do
    {
        while(stringToSort[i] < pivot)
        {
            i = i + 1;
        }

        while(stringToSort[j] > pivot)
        {
            j = j - 1;
        }

        if(i <= j)
        {
            pom = stringToSort[i];
            stringToSort[i] = stringToSort[j];
            stringToSort[j] = pom;

            i = i + 1;
            j = j - 1;
        }
    }
    while(i <= j);

    if (left < j)
        quickSort(stringToSort, left, j);
    if (i < right)
        quickSort(stringToSort, i, right);

    return;
}


/*
 * Pripravuje pamet a meze retezce pro samotny beh quickSortu, ktery pak vola
 */
tBTSUzolPtr prep_quickSort(tBTSUzolPtr input)
{
    if(error != EOK)
        return NULL;

    int goOn = 1;
    int start = 0;
    char* stringToSort;
    char* cpStringToSort;
    int end = 0;

    if(input == NULL || input->data.typ != tString)   // jestli se jedna opravdu o string, jinak chyba
    {
        goOn = 0;
    }
    if(goOn)
    {
        stringToSort = input->data.value.s;
        start = 0;
        end = strlen(stringToSort) - 1;

        cpStringToSort = advMalloc((end+1) * sizeof(char));

        strcpy(cpStringToSort, stringToSort);
        quickSort(cpStringToSort, start, end);
    }

    // ulozeni struktury
    char* str = advMalloc(sizeof(char)*25);
    sprintf(str,"@prom_%u",nameID++);

    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazov = str;
    ptr.varFc = false;
    if(goOn)
    {
        ptr.value.s = cpStringToSort;
        ptr.typ = tString;
    }
    else
    {
        ptr.typ =tNil;
    }

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ESEM;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);

    return node;

}


tBTSUzolPtr find(tBTSUzolPtr input, tBTSUzolPtr inputToFind)
{
    // SKRIPTA: 'P' - stringToFind (podretezec)
    //          'T' - string (v cem hledam)

    if(error != EOK)
        return NULL;

    int result = 0;
    int goOn = 1;

    if((input == NULL) || (inputToFind == NULL))   // jestli se jedna opravdu o string, jinak chyba
    {
        goOn = 0;      // preskoci to hledani, a vrati NIL
        result = 0;

    }
    if((goOn == 1) && ((input->data.typ != tString) || (inputToFind->data.typ != tString)))   // jestli se jedna opravdu o string, jinak chyba
    {
        goOn = 0;      // preskoci to hledani, a vrati NIL
        result = 0;
    }

    if(goOn == 0) {

            char* str = advMalloc(sizeof(char)*25);
            sprintf(str,"@prom_%u",nameID++);

        #ifdef DEBUG

        #endif

            tSymbol ptr;
            TSinitSymbol(&ptr);
            ptr.nazov = str;
            ptr.varFc = false;

             ptr.typ = tNil;

        if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
            error = ESEM;
        else TSvlozSymbol(ptr); //jinak vloz do tabulky

        tBTSUzolPtr node = TSreadSymbol(str);
        return node;
    }

    char* string = input->data.value.s;
    char* stringToFind = inputToFind->data.value.s;

#ifdef DEBUG

#endif

    int charJump[MAX];
    int matchJump[strlen(stringToFind)];
    goOn = 1;

    int lenStr = strlen(string);
    int lenStr_j = strlen(stringToFind)-1;
    int lenStr_k = lenStr_j;
    int lenStrToFind = lenStr_j+1;

    if (lenStrToFind == 0)    //pokud je pattern prazdny retezec
    {
        result = 0;
        goOn = 0;
    }
    if (goOn == 1 && lenStrToFind == 1)    //pokud je retezec jen 1 => porovnej to jen symbol k symbolu - urychleni
    {
        for(int w = 0; w < lenStr; w++)
        {
            if(stringToFind[0] == string[w])
                result = w+1;
            goOn = 0;
        }
    }
    if(goOn == 1)
    {
        // predpriprava pred porovnavanim
        calcCharJumps(charJump, stringToFind, lenStrToFind);
        calcMatchJumps(matchJump, stringToFind, lenStrToFind);

        // hledani podretezce
        while (lenStr_k >= 0 && lenStr_j < lenStr)
        {
            if (stringToFind[lenStr_k] == string[lenStr_j])		// pokud dojde ke shode, kontrolujeme dalsi znak vlevo
            {
                lenStr_j--;
                lenStr_k--;
            }
            else    // nedoslo ke shode
            {
                lenStr_j = lenStr_j + (charJump[(int)string[lenStr_j]] > matchJump[lenStr_k] ? charJump[(int)string[lenStr_j]] : matchJump[lenStr_k]);	// pokud se nerovnaji, skocime dal podle vyhodnejsi heurestiky
                lenStr_k = lenStrToFind - 1;
            }
        }

        if (lenStr_k == -1)
        {
            result = lenStr_j + 2;					// + 2 <= index v poli od 0 a doslo k posunu o -1
        }
        else
        {
            // -1 jako nenalezeno
            result = -1;
        }
    }
    // ulozeni struktury
    char* str = advMalloc(sizeof(char)*25);
    sprintf(str,"@prom_%u",nameID++);

#ifdef DEBUG

#endif

    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazov = str;
    ptr.varFc = false;
    if(result == -1)  // vzorek nenalezen
    {
        ptr.typ = tBool;
        ptr.value.b = false;
    }
    else if (result > 1)  // vzorek nalezen
    {
        ptr.typ = tDouble;
        ptr.value.d = result;
    }
    else    // spatne parametry
    {
        ptr.typ = tNil;
    }

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ESEM;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);

    return node;
}


// 1. typ heurestiky pro Boyer-Moore alogritmus
void calcCharJumps(int charJump[], char* stringToFind, int len_stringToFind)
{
    int q;

    for(q = 0; q < MAX; q++)
    {
        charJump[q] = len_stringToFind;
    }
    for(q = 0; q <= len_stringToFind; q++)
    {
        charJump[(int)stringToFind[q]] = len_stringToFind - 1 - q ; // -1 kvuli indexaci do pole
    }

    return;
}

// 2. typ heurestiky pro Boyer-Moore alogritmus
void calcMatchJumps(int matchJump[], char *patCont, int m)
{
    // q == pos2; k == pos; m == Len

    int k,q,qq;
    int backup[m+1];

    for (q = 0; q < m + 1; q++)
    {
        backup[q] = 0;
        matchJump[q] = 0;
    }

    k = m;
    q = m+1;

    while (k > 0)
    {
        backup[k] = q;

        while ((q <= m) && (patCont[k-1] != patCont[q-1]))
        {
            matchJump[q] = (matchJump[q] > m - k ? matchJump[q] : m-k);
            q = backup[q];
        }
        k = k - 1;
        q = q - 1;
    }

    for(k = 0; k < q; k++)
    {
        matchJump[k] = (matchJump[k] > m - k ? matchJump[q] : m-k);
    }

    qq = backup[q];
    while(q < m)
    {
        while(q < qq)
        {
            matchJump[q] = (matchJump[q] > qq-q+m ? matchJump[q] : qq-q+m);
            q = q + 1;
        }
        qq = backup[qq];
    }
    return;
}
