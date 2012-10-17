/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   lexikalny_analyzator.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

/**
 * Hlavickove subory
 */
#include <ctype.h>
#include "garbage_collector.h"
#include "lexikalny_analyzator.h"
#include "funkcie.h"

/**
 * Konstanty
 */
#define POCET_KLUCOVYCH_SLOV 14
#define POCET_REZERVOVANYCH_SLOV 9

/**
 * Globalna premenna
 */
tToken token;

/**
 * Prototypy funkcii
 */
static void initToken(void);                        // inicializuje token
static void rozsirToken(int znak, int *i);          // aktualizuje hodnotu
static void naplnToken(tStav stav);                 // naplni token datami
static void vratZnak(int znak);                     // vrati znak do buffru
static tStav prejdiRezervovaneSlova(char *slovo);   // prezre rezerv slova

/**
 * Klucove slova
 */
const char *klucoveSlova[POCET_KLUCOVYCH_SLOV] =
{
    "do\0", "else\0", "end\0", "false\0", "function\0", "if\0", "local\0",
    "nil\0", "read\0", "return\0", "then\0", "true\0", "while\0", "write\0"
};

/**
 * Rezervovane slova
 */
const char *rezervovaneSlova[POCET_REZERVOVANYCH_SLOV] =
{
    "and\0", "break\0", "elseif\0", "for\0", "in\0", "not\0", "or\0",
    "repeat\0", "until\0"
};

/**
 * Funkcia vlozi znak do datovej zlozky tokenu cim rozsiri jeho hodnotu
 *
 * @param znak Vkladany znak
 * @param i Ukazatel na pocitadlo sparsovanych znakov
 */
static void rozsirToken(int znak, int *i)
{
    // ak sa nam podari (re)alokovat pamet
    if ((token.data = (char *) advRealloc(token.data, (*i) + 2)))
    {
        token.data[(*i) + 1] = '\0';    // presunieme koncovy znak
        token.data[(*i)] = znak;        // vlozime novy znak
        (*i)++;                         // inkrementujeme pocitadlo znakov
    }
    else
        error = EINTERN;
}

/**
 * Vrati dany znak do bufferu a opravi stlpec
 *
 * @param znak Vracany znak
 */
static void vratZnak(int znak)
{
    // ak sa nejedna o prazdny znak tak ho vratime
    if (!isspace(znak))
        ungetc(znak, subor);

    // aktualizujeme stlpec
    if (isprint(znak))
        stlpec--;
}

/**
 * Medzistav pre kontrolu rezervovanych slov
 *
 * @param ident Skumane slovo
 */
static tStav prejdiRezervovaneSlova(char *slovo)
{
    int j;

    // prejdeme klucove slova
    for (j = 0; j < POCET_KLUCOVYCH_SLOV; j++)
        if (!(strcmp(slovo, klucoveSlova[j])))
            return S_KLUC;

    // prejdeme prezervovane slova
    for (j = 0; j < POCET_REZERVOVANYCH_SLOV; j++)
        if (!(strcmp(slovo, rezervovaneSlova[j])))
            return S_REZ;

    return S_IDENT;
}

/**
 * Inicializuje token
 */
static void initToken(void)
{
    // token je globalny
    token.stav = S_START;
    token.data = NULL;
    token.riadok = riadok;
    token.stlpec = stlpec;
}

/**
 * Naplni token
 */
static void naplnToken(tStav stav)
{
    token.stav = stav;
}

/**
 * Funkcia na ziskanie tokenu zo zdrojoveho suboru
 */
tToken getToken(void)
{
    // deklarujeme a inicializujeme
    tStav stav = S_START;   // minuly a sucasny stav
    bool pokracuj = true;   // predikat lexanu
    int i = 0;              // pocitadlo sparsovanych znakov
    int c;                  // nacitany znak

    // inicializujeme token
    initToken();

    // pustime automat
    while (pokracuj && (c = getc(subor)))
    {
        switch (stav)
        {
        case S_START:               // 00 - pociatocny stav
        {
            token.riadok = riadok;
            token.stlpec = stlpec;

            if ((isalpha(c)) || (c == '_'))     stav = S_IDENT;
            else if (isdigit(c))                stav = S_INT;
            else if (c == '=')                  stav = S_PRIR;
            else if (c == ';')                  stav = S_STREDNIK;
            else if (c == ',')                  stav = S_CARKA;
            else if (c == '.')                  stav = S_BODKA;
            else if (c == '(')                  stav = S_LZ;
            else if (c == ')')                  stav = S_PZ;
            else if (c == '+')                  stav = S_ADD;
            else if (c == '-')                  stav = S_SUB;
            else if (c == '*')                  stav = S_MUL;
            else if (c == '/')                  stav = S_DIV;
            else if (c == '^')                  stav = S_MOC;
            else if (c == '~')                  stav = S_TIL;
            else if (c == '>')                  stav = S_VEC;
            else if (c == '<')                  stav = S_MEN;
            else if (c == '!')                  stav = S_VYKR;
            else if (c == EOF)                  stav = S_ENDOFFILE;
            else if (c == '"')
            {
                stav = S_STR;
                break;
            }
            else if (isspace(c))
            {
                stav = S_START;
                break;
            }
            else
            {
                stav = S_CHYBA;
                break;
            }

            rozsirToken(c, &i);        // vlozime znak do tokenu
            break;                              // nacitame dalsi znak
        }

        case S_IDENT:               // 06 - identifikator
        {
            // identifikator moze pozostavat zo znakov cislic a podtrzitka
            if ((isalpha(c) || isdigit(c) || (c == '_')))
            {
                stav = S_IDENT;                 // zotrvavame v tomto stave
                rozsirToken(c, &i);    // rozsirime token o znak
            }
            // ak dalsi znak nevyhovuje docitali sme token
            else
            {
                // kedze sa jedna o identifikator pozrieme sa ci sa nejedna
                //      o klucove alebo rezervovane slovo a nastavime token
                token.stav = prejdiRezervovaneSlova(token.data);

                stav = S_KONEC;                 // nastavime koncovy stavu
                vratZnak((char) c);             // znak vratime
            }

            break;
        }

        case S_INT:                 // 07 - cele cislo
        {
            if (isdigit(c))
            {
                stav = S_INT;
                rozsirToken(c, &i);
            }
            else if (c == '.')  // bodka cize double
            {
                stav = S_DOUBLEP;   // musi nasledovat cislo
                rozsirToken(c, &i);
            }
            else if ((c == 'e') || (c == 'E'))
            {
                stav = S_EXPOP;     // e cize exponencialne
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_DOUBLEP:             // 09 - medzistav double
        {
            if (isdigit(c))
            {
                stav = S_DOUBLE;            // za bodkou nasleduje
                rozsirToken(c, &i); // cislo cize pokracujeme
            }
            else
            {
                naplnToken(stav);
                stav = S_CHYBA;         // S_DOUBLEP nie je koncovy stav
                vratZnak((char) c);    // cize skaceme do S_CHYBA
            }

            break;
        }

        case S_DOUBLE:              // 08 - double
        {
            if (isdigit(c))
            {
                stav = S_DOUBLE;
                rozsirToken(c, &i);
            }
            else if ((c == 'e') || (c == 'E'))  // e cize exponencialne
            {
                stav = S_EXPOP;
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;         // S_DOUBLE je koncovy stav
                vratZnak((char) c);    // cize skaceme do S_KONEC
            }

            break;
        }

        case S_EXPOP:               // 11 - medzistav exp
        {
            if ((c == '-') || (c == '+'))
            {
                stav = S_EXPOP2;     // musi nasledovat cislo
                rozsirToken(c, &i);
            }
            else if (isdigit(c))
            {
                stav = S_EXPO;      // kladny implcitne
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_CHYBA;
                vratZnak((char) c);
            }

            break;
        }

        case S_EXPOP2:              // 12 - medzistav exp 2
        {
            if (isdigit(c))
            {
                stav = S_EXPO;
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_CHYBA;
                vratZnak((char) c);
            }

            break;
        }

        case S_EXPO:                // 10 - exponencialne
        {
            if (isdigit(c))
            {
                stav = S_EXPO;
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_ADD:                 // 20 - +
        {
            naplnToken(stav);
            stav = S_KONEC;
            vratZnak((char) c);

            break;
        }

        case S_SUB:                 // 21 - -
        {
            if (c == '-')           // -- riadkovy komentar
            {
                stav = S_RCOM;
            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_PRIR:                // 14 - =
        {
            if (c == '=')
            {
                stav = S_POROV;
                rozsirToken(c, &i);
            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_TIL:                 // 25 - ~
        {
            if (c == '=')
            {
                stav = S_TILROV;
                rozsirToken(c, &i);

            }
            else
            {
                naplnToken(stav);
                stav = S_CHYBA;
                vratZnak((char) c);
            }

            break;
        }

        case S_VEC:                 // 26 - >
        {
            if (c == '=')
            {
                stav = S_VECROV;
                rozsirToken(c, &i);

            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_MEN:                 // 27 - <
        {
            if (c == '=')
            {
                stav = S_MENROV;
                rozsirToken(c, &i);

            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_VYKR:                // 28 - !
        {
            if (c == '=')
            {
                stav = S_NEROV;
                rozsirToken(c, &i);

            }
            else
            {
                naplnToken(stav);
                stav = S_KONEC;
                vratZnak((char) c);
            }

            break;
        }

        case S_BODKA:               // 17 - .
        {
            if (c == '.')
            {
                stav = S_DVOJB;
                rozsirToken(c, &i);

            }
            else
            {
                naplnToken(stav);
                stav = S_CHYBA;
                vratZnak((char) c);
            }

            break;
        }

        case S_STR:                 // 13 - string
        {
            if (c == '\\')          // escape sekvencia \" v komentari
            {
                stav = S_STRP;
                rozsirToken(c, &i);
            }
            else if (c != '"')
            {
                stav = S_STR;
                rozsirToken(c, &i);
            }
            else                    // koniec retazca
            {
                token.stav = S_STR;
                stav = S_KONEC;
            }

            break;
        }

        case S_STRP:                // 29 - pomocny stav pre escape \"
        {
            stav = S_STR;
            rozsirToken(c, &i);
            break;
        }

        case S_RCOM:                // 36 - --
        {
            if (c == '\n')
            {
                stav = S_START;
                i = 0;
            }
            else if (c == '[')
                stav = S_RCOMHZ;
            else if (c == EOF)
                stav = S_ENDOFFILE;
            else
                stav = S_RCOMD;

            break;
        }

        case S_RCOMD:               // 37 - --x
        {
            if (c == '\n')
            {
                stav = S_START;
                i = 0;
            }
            else if (c == EOF)
                stav = S_ENDOFFILE;
            else
                stav = S_RCOMD;

            break;
        }

        case S_RCOMHZ:              // 38 - --[
        {
            if (c == '[')
                stav = S_BCOMZ;
            else if (c == EOF)
                stav = S_ENDOFFILE;
            else
                stav = S_RCOMD;

            break;
        }

        case S_BCOMZ:               // 39 - --[[
        {
            if (c == ']')
                stav = S_BCOMK;
            else if (c == EOF)
                stav = S_ENDOFFILE;
            else
                stav = S_BCOMZ;

            break;
        }

        case S_BCOMK:               // 40 - --[[x]
        {
            if (c == ']')
            {
                stav = S_START;
                i = 0;
            }
            else if (c == EOF)
                stav = S_ENDOFFILE;
            else
                stav = S_BCOMZ;

            break;
        }

        // konecne stavy
        case S_KLUC:                // 01
        case S_REZ:                 // 02
        case S_STREDNIK:            // 15 - ;
        case S_CARKA:               // 16 - ,
        case S_LZ:                  // 18 - (
        case S_PZ:                  // 19 - )
        case S_MUL:                 // 22 - *
        case S_DIV:                 // 23 - /
        case S_MOC:                 // 24 - ^
        case S_DVOJB:               // 30 - ..
        case S_POROV:               // 31 - ==
        case S_NEROV:               // 32 - !=
        case S_TILROV:              // 33 - ~=
        case S_VECROV:              // 34 - >=
        case S_MENROV:              // 35 - <=
        case S_ENDOFFILE:           // 03 - EOF
        {
            naplnToken(stav);
            stav = S_KONEC;
            vratZnak((char) c);
            break;
        }
        case S_CHYBA:               // 04 - chyba
        {
            error = ELEX;
            pokracuj = false;
            break;
        }
        case S_KONEC:               // 05 - koncovy stav
        {
            vratZnak((char) c);
            pokracuj = false;
            break;
        }
        }

        // ak nastala chyba vyskocime z lexikalneho analyzatoru
        if (error)
            break;

        // aktualizujeme pocitadlo riadkov a stlpcov pre dalsi cyklus
        if (c == '\n')
        {
            riadok++;
            stlpec = 1;
        }
        else if (isprint(c))
            stlpec++;
    }

    return token;
}
