/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka
 * Variant: b/1/I
 * Subor:   syntakticky_analyzator.c
 * Datum:   10.10.2011
 * Autori:  Bambusek David  <xbambu02@stud.fit.vutbr.cz>,
 *          Klvana Martin   <xklvan00@stud.fit.vutbr.cz>,
 *          Konar David     <xkonar07@stud.fit.vutbr.cz>,
 *          Marecek Matej   <xmarec12@stud.fit.vutbr.cz>,
 *          Szabo Peter     <xszabo06@stud.fit.vutbr.cz>
 */

/**
 *  Hlavickove subory
 */
#include <ctype.h>
#include <math.h>
#include "funkcie.h"
#include "garbage_collector.h"
#include "syntakticky_analyzator.h"
#include "lexikalny_analyzator.h"
#include "vyrazy.h"
#include "tagenerator.h"
#include "interpret.h"

#define SORT_PARAMS 1
#define FIND_PARAMS 2
#define TYPE_PARAMS 1
#define SUBSTR_PARAMS 3
tSymbol name;
tData neterm;
tData fce;
TA_Item *ta;
tBTSUzolPtr ts;
bool isExprWr = false;
bool isWrite = false;
bool isType = false;
bool isSort = false;
bool isFind = false;
bool isSubs = false;
bool isLocal = false;
bool isWriteFunc = false;
tBTSUzolPtr typPrirazeni = NULL;
int nazev_fce;
tBTSUzolPtr volana_fce=NULL;
int label=1;
tBTSUzolPtr cilovaAdresa=NULL;
tBTSUzolPtr cilovaAdresaFce=NULL;
tBTSUzolPtr for_return=NULL;
tBTSUzolPtr pole_argumentu[4] = {NULL, NULL, NULL, NULL};
int *cislo_argumentu=NULL;
int *pole_back=NULL;
int index_pb=0;



tBTSUzolPtr functionNode; //ukazatel na funkci, ktere chci dat argumenty
tBTSUzolPtr functionNodeTmp;
tBTSUzolPtr pom;
char *functionName = NULL;

tError parser()
{

    ta_Initialize(&ta);
    cislo_argumentu=advMalloc(sizeof(int));
    *(cislo_argumentu)=1;

    pole_back=(int*)advMalloc(sizeof(int)*ALL_MORE);
    error = EOK;
    tToken token = getToken();

    if(token.stav == S_ENDOFFILE) //pokud je soubor prazdny, chyba
        error = ESYN;
    else error = program(); //pokracuj v analyze

    if(error != EOK)
        return error;

    interpret(&ta, ts);

    ta_Destroy(&ta);
    return error;
}

/**
 * <program> 	->  <func> ; EOF
 */

tError program()
{
    if(error != EOK) //pokud je vse v poradku, tak pokracuj
        return error;
    //isWrite = false; //nevolame zatim funkci write

    ta_InsertJump(&ta,I_JUMP,NULL,0);
    //ta->end = 0;

    if(token.stav != S_KLUC) //pokud nedostanu klicove slovo
        return  ESYN;

    error = funkce();
    if(error != EOK)
        return error;

    getToken();

    if(token.stav == S_KLUC)
    {
        if(!(strcmp(token.data, "function"))) //v pripade ze main nebyl posledni -> chyba
            error = ESEM;
    }

    if(token.stav != S_ENDOFFILE) //kod musi konci EOF
        error = ESYN;


    return error;
}

/**
 * <func>	->	EPS
 * <func>		->	"function" ID "(" <params> ")" <vardec> <list> "end" <func>
 */

tError funkce()
{
    if(error != EOK)
        return error;

    if(token.stav != S_KLUC) //pokud nedostanu klicove slovo
        return  ESYN;

    if((strcmp(token.data, "function"))) //"function"
        return ESYN;

    getToken();
    if(token.stav != S_IDENT) //pokud nedostanu identifikator
        return ESYN;

    if(!(strcmp(token.data, "main"))) //pokud mame "main"
    {
        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = true;
        char *nazev = token.data; //vlozim data
        dataStromu.nazov = nazev;
        dataStromu.jump=label;
        label++;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            return ESEM;
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNode = TSreadSymbol(nazev);
        functionName = functionNode->data.nazov; //udelam si odkaz na jmeno soucasne funkce

        if(error != EOK)
            return error;

        najdiKolize(ts);
        if(error != EOK)
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return error;
        }


        getToken();
        if(token.stav != S_LZ) // "("
            return ESYN;
        getToken();
        if(token.stav != S_PZ) // ")"
            return ESYN;

        ta_InsertJump(&ta,I_LABEL,NULL,0);

        getToken();
        if(error != EOK)
            return error;

        if((strcmp(token.data, "end"))) //pokud funkce neni prazdna
            error = body(); //<body>


        if(error != EOK)
            return error;

        if(token.stav == S_KLUC) //klicove slovo
        {
            if((strcmp(token.data, "end"))) //"end"
                return ESYN;

            getToken();
            if(error != EOK)
                return error;

            if(token.stav != S_STREDNIK) //";"
                return ESYN;
        }
        else return ESYN;

        return error;
    }
    else //jinak jina funkce
    {
        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = true;
        char *nazev = token.data; //vlozim data
        dataStromu.nazov = nazev;
        dataStromu.jump=label;

        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            return ESEM;
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNode = TSreadSymbol(nazev);
        functionName = functionNode->data.nazov; //udelam si odkaz na jmeno soucasne funkce
        functionNodeTmp = functionNode;
        for_return=functionNode;

        if(error != EOK)
            return error;

        najdiKolize(ts);
        if(error != EOK)
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return error;
        }

        getToken();
        if(token.stav != S_LZ) // "("
            return ESYN;

        error = params(); //<params>
        if(error != EOK)
            return error;

        if(token.stav != S_PZ) // ")"
            return ESYN;

        ta_InsertJump(&ta,I_LABEL,NULL,label);
        label++;

        getToken();
        if(error != EOK)
            return error;

        if((strcmp(token.data, "end"))) //pokud funkce neni prazdna
            error = body(); //<body>

        if(error != EOK)
            return error;

        if(token.stav == S_KLUC) //klicove slovo
        {
            if((strcmp(token.data, "end"))) //"end"
                return ESYN;
        }

        ta_Insert(&ta,I_GOTO,NULL,NULL,NULL);

        getToken();
        if(error != EOK)
            return error;

        if(token.stav == S_KLUC) //pokud nedostanu klicove slovo
            if(!(strcmp(token.data, "function"))) //"function"
                error = funkce(); //jdu zpracovat dalsi funkci
    }

    return error;
}

/**
 *  overim jestli jsou ve funkci deklarovane promenne a prikazy
 */
tError body()
{
    if(error != EOK)
        return error;

    error = varDeclar(); //<varDeclar>
    if(error != EOK)
        return error;

    if(!(strcmp(token.data, "end"))) //pokud dostanu "END"
        return error;

    error = list(); //<list>
    if(error != EOK)
        return error;

    return error;
}

/**
 * <list>        ->  EPS
 * <list>	->  ID <prirazeni> ";" <list>
 * <list>	->  "write" <write> <list>
 * <list>	->  "if" <expr> "then" <list> "else" <list> "end" ";" <list>
 * <list>	->  "while" <expr> "do" <list> "end" ";" <list>
 * <list>	->  "return" <expr> ";" <list>
 */

tError list()
{
    if(error != EOK)
        return error;

    if(!(strcmp(token.data, "end"))) //list je na konci
        return error;

    if(!(strcmp(token.data, "else"))) //list je na konci "then" u "IF"
        return error;

    tSymbol dataStromu; //vlozim do stromu jmeno funkce
    TSinitSymbol(&dataStromu);
    char *nazev;
    tBTSUzolPtr tmpNode = NULL;

    switch (token.stav)
    {
    case S_IDENT: //<list>	->  ID <prirazeni> ";" <list>

        tmpNode = TSreadSymbol(token.data); //pokud najdes, tak to je funkce
        if((tmpNode != NULL) && (tmpNode->data.varFc == true))
        {
            volana_fce=tmpNode;
            getToken();
            if(error != EOK)
                return error;

            if(token.stav != S_LZ) //musim mit "("
                return ESYN;

            error = args();
            if(error != EOK)
                return error;

            if(token.stav != S_PZ) //musim mit ")"
                return ESYN;


            int kam=tmpNode->data.jump;
            ta_InsertJump(&ta,I_FCEJUMP,NULL, kam);


            getToken();
            if(error != EOK)
                return error;


            if(token.stav != S_STREDNIK) //musim mit ";"
                return ESYN;

            break;
        }
        else if((!(strcmp(token.data, "type"))) || (!(strcmp(token.data, "substr"))) || (!(strcmp(token.data, "find"))) || (!(strcmp(token.data, "sort"))))
        {
            error = vest();
            if(error != EOK)
                return error;
            break;
        }

        nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne

        tmpNode = TSreadSymbol(nazev);
        if(tmpNode == NULL) //jmeno neni v tabulce
        {
            return ESEM;
        }

        cilovaAdresa = tmpNode;

        getToken();
        if(error != EOK)
            return error;

        if(token.stav != S_PRIR) //musim mit "="
        {
            fprintf(stderr, "Ocekavan znak: \"=\"\n");
            return ESYN;
        }

        error = prirazeni();
        if(error != EOK)
            return error;

        break;

    case S_KLUC: //klicove slovo
        if(!(strcmp(token.data, "read"))) //read musi byt prirazeny
            error = ESYN;

        if(!(strcmp(token.data, "write"))) //<list>		->  "write" <write> <list>
            error = write();

        else if(!(strcmp(token.data, "if"))) //<list>	->  "if" <expr> "then" <list> "else" <list> "end" ";" <list>
        {
            int else_label=(label);
            int end_label=++(label);
            (label)++;
            getToken(); //EXPR
            if(error != EOK)
                return error;

            error = pparser(); //vyhodnotim podminku

            ta_InsertJump(&ta,I_FJUMP,TSreadSymbol(neterm.data.nazov),else_label);

            if(error != EOK)
                return error;

            if((strcmp(token.data, "then")))
                return ESYN;

            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;

            ta_InsertJump(&ta,I_JUMP,TSreadSymbol(neterm.data.nazov),end_label);

            ta_InsertJump(&ta,I_LABEL,NULL, else_label);


            if((strcmp(token.data, "else")))
                return ESYN;

            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;;

            ta_InsertJump(&ta,I_LABEL,NULL, end_label);

            if((strcmp(token.data, "end"))) //"end"
                return ESYN;

            getToken(); //";"
            if(error != EOK)
                return error;

            if(token.stav != S_STREDNIK) //";"
            {
                fprintf(stderr, "Ocekavan znak: \";\"\n");
                return ESYN;
            }

        }
        else if(!(strcmp(token.data, "while"))) //<list>	->  "while" <expr> "do" <list> "end" ";" <list>
        {
            int while_label=(label);
            int while_end_label=++(label);
            label++;

            getToken(); //EXPR
            if(error != EOK)
                return error;

            ta_InsertJump(&ta,I_LABEL,NULL, while_label);

            error = pparser();//////////////////////////////////

            ta_InsertJump(&ta,I_FJUMP,TSreadSymbol(neterm.data.nazov),while_end_label);

            if(error != EOK)
                return error;

            if(error != EOK)
                return error;

            if((strcmp(token.data, "do")))
                return ESYN;

            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;

            ta_InsertJump(&ta,I_JUMP,TSreadSymbol(neterm.data.nazov),while_label);

            if((strcmp(token.data, "end"))) //"end"
                return ESYN;

            ta_InsertJump(&ta,I_LABEL,NULL, while_end_label);

            getToken(); //";"
            if(error != EOK)
                return error;

            if(token.stav != S_STREDNIK) //";"
            {
                fprintf(stderr, "Ocekavan znak: \";\"\n");
                return ESYN;
            }

        }

        else if(!(strcmp(token.data, "return"))) //<list>	->  "return" <expr> ";" <list>
        {
            getToken(); //EXPR
            if(token.stav != S_STREDNIK)   //";"
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazov), NULL, for_return);

                if(error != EOK)
                    return error;

                if(token.stav != S_STREDNIK) //";"
                {
                    fprintf(stderr, "Ocekavan znak: \";\"\n");
                    return ESYN;
                }
            }
            ta_Insert(&ta, I_GOTO,NULL,NULL,NULL);
        }
        else return ESYN;

        if(error != EOK)
            return error;

        break;

    default: //pokud je cokoli jineho
        return ESYN;
    }

    getToken();
    if(error != EOK)
        return error;

    error = list();
    if(error != EOK)
        return error;

    return error;
}

/**
 * <write>		->	"(" <expr> ")" ";"
 */
tError write()
{
    getToken();
    if(error != EOK)
        return error;

    if(token.stav != S_LZ) //musi byt "("
        return ESYN;

    isWrite = true; //zapamatuji si, ze jsem ve funkci write

    error = args();

    if(error != EOK)
        return error;

    getToken();
    if(error != EOK)
        return error;

    if(token.stav != S_STREDNIK) //musi byt ";"
        return ESYN;


    isWrite = false; //uz nejsem ve funkci write, tak nevyhodnocuji vyrazy
    return error;
}

/**
 * <read>	->  "read" "(" format ")" ";"
 */
tError read()
{
    nazev_fce=1;
    getToken();
    if(error != EOK)
        return error;

    if(token.stav != S_LZ) //musi byt "("
        return ESYN;

    getToken();
    if(error != EOK)
        return error;

    if((token.stav == S_STR) || (token.stav == S_INT) || (token.stav == S_DOUBLE) || (token.stav == S_EXPO)) //musi byt STRING nebo CISLO
    {

        nameID++; //prictu k pocitadlu
        char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
        sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        dataStromu.nazov = nazev;
        dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
        if(token.stav == S_STR)
        {
            dataStromu.value.s=token.data;
            dataStromu.typ = tString; //typ je string
        }
        else
        {
            dataStromu.typ = tDouble; //typ je double
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
        }
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        ta_Insert(&ta, I_READ, TSreadSymbol(nazev),NULL, cilovaAdresa);

        getToken();
        if(error != EOK)
            return error;

        if(token.stav != S_PZ) //musi byt ")"
            return ESYN;

        getToken();
        if(error != EOK)
            return error;

        if(token.stav != S_STREDNIK) //musi byt ";"
            return ESYN;

        return error;
    }
    else return ESEM;
}


/**
 * <vest>			->	"substr" "(" string, number, number ")" ";"
 * <vest>			->	"find" "(" string, string ")" ";"
 * <vest>			->	"type" "(" ID ")" ";"
 * <vest>			->	"sort" "(" string ")" ";"
 */

tError vest()
{
    if(error != EOK)
        return  error;

    pole_argumentu[0] = NULL;
    pole_argumentu[1] = NULL;
    pole_argumentu[2] = NULL;
    pole_argumentu[3] = NULL;

    int i = 0;

    if(isExprWr)
        isWrite = false;

    if(!(strcmp(token.data, "sort"))) i = 1;
    else if(!(strcmp(token.data, "type"))) i = 2;
    else if(!(strcmp(token.data, "find"))) i = 3;
    else if(!(strcmp(token.data, "substr"))) i = 4;

    else return ESYN; //token neni vestavena funkce

    getToken();
    if(error != EOK)
        return error;

    if(token.stav != S_LZ) //musi byt "("
        return ESYN;

    switch(i)
    {
    case 1: //Sort();
        isSort=true;
        error = args();

        if(error != EOK)
            return error;

        ta_Insert(&ta, I_SORT ,pole_argumentu[0], NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data=cilovaAdresa->data;
        break;


    case 2: //Type();
        isType=true;

        error = args();

        if(error != EOK)
            return error;

        ta_Insert(&ta, I_TYPE ,pole_argumentu[0], NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data=cilovaAdresa->data;
        break;


    case 3: //Find();
        isFind=true;
        error = args();
        if(error != EOK)
            return error;

        ta_Insert(&ta, I_FIND , pole_argumentu[0], pole_argumentu[1], cilovaAdresa);
        if(isExprWr)
            neterm.data=cilovaAdresa->data;
        break;

    case 4: //substr();
        isSubs=true;
        error = args();
        if(error != EOK)
            return error;

        ta_Insert(&ta, I_SUBS , pole_argumentu[0], pole_argumentu[1], pole_argumentu[2]);
        ta_Insert(&ta, I_SUBS , NULL, NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data=cilovaAdresa->data;
        break;

    default: //tento pripad by snad nemel ani nikdy nastat... :-/
        return ESYN;
    }
    isType=false;
    isSort=false;
    isSubs=false;
    isFind=false;
    (*cislo_argumentu)=1;
    if(token.stav != S_PZ) //musi byt ")"
        return ESYN;

    getToken();
    if(error != EOK)
        return error;

    if(isExprWr)
    {
        isWrite = true;
        if((token.stav != S_CARKA)&&(token.stav != S_PZ)) //musi byt ","
            return ESYN;
    }
    else if(token.stav != S_STREDNIK) //musi byt ";"
        return ESYN;

    return error;
}

/**
 * <vardec>	->	EPS
 * <vardec>	->	"local" ID <idt> ";" <vardec>
 */

tError varDeclar()
{
    if(token.stav != S_KLUC) //zrejme nejsou zadne deklarace
        return error;

    if((strcmp(token.data, "local"))) //nejsou zadne deklarace
        return error;

    getToken();
    if(error != EOK)
        return error;

    if(token.stav != S_IDENT) //ID... za local musi byt ID
        return ESYN;

    if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
        return ESEM;

    tSymbol dataStromu; //vlozim do stromu jmeno funkce
    TSinitSymbol(&dataStromu);
    dataStromu.varFc = false;
    char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
    sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
    name.nazov=nazev;
    dataStromu.nazov = nazev;
    if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
    {
        fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
        return ESEM;
    }
    else TSvlozSymbol(dataStromu); //jinak vloz do tabulky
    tBTSUzolPtr tmpNode = TSreadSymbol(nazev);

    getToken();
    if(error != EOK)
        return error;

    if(token.stav == S_STREDNIK) //";" by strednik
    {
        getToken();
        if(error != EOK)
            return error;
        error = varDeclar(); //<varDeclar> kdyby byly dalsi promenne
        return error;
    }
    else
    {
        cilovaAdresa = tmpNode;
        if(token.stav == S_PRIR)   //"=" prirazeni
        {
            isLocal = true;
            error = prirazeni();
            isLocal = false;
        }
        else return ESYN; //jinak nastala chyba

        getToken();
        if(error != EOK)
            return error;
        error = varDeclar(); //<varDeclar> kdyby byly dalsi promenne
    }

    return error;
}


/**
 * <prirazeni>		->	"=" <litexpr>
 */
tError prirazeni()
{
    if(error != EOK)
        return error;

    if(token.stav != S_PRIR)//"="
        return error;

    error = litExpr();
    if(error != EOK)
        return error;

    return error;
}

/**
 * <litexpr>		->	<expr>	";"
 * <litexpr>		->	<vest>
 * <litexpr>		->	<read>
 * <litexpr>		->	ID"(" ")"
 */
tError litExpr()
{
    if(isWriteFunc == false)
        getToken();
    if(error != EOK)
        return error;

    tBTSUzolPtr tmpNode = NULL;// = TSreadSymbol(nazev);
    tBTSUzolPtr tmpNode2 = NULL;// = TSreadSymbol(token.data); //pokud je token se jmenem funkce
    char *nazev = NULL;

    switch (token.stav)
    {
    case S_KLUC: //<litexpr> -> <read>
        if(!(strcmp(token.data, "read")))
        {
            error = read();
            break;
        }

    case S_IDENT: //ID je ve skutecnosti <litexpr> -> <vest>
        if(!(strcmp(token.data, "sort")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        else if(!(strcmp(token.data, "type")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        else if(!(strcmp(token.data, "find")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }
        else if(!(strcmp(token.data, "substr")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        if(error != EOK) //nekde ve <vest> nastala chyba
            return error;

    default: //pokud to neni read ani vestavena funkce, tak to musi byt <EXPR>

        if(token.stav == S_IDENT)
        {
            tmpNode2 = TSreadSymbol(token.data); //pokud je token se jmenem funkce
            if(tmpNode2 == NULL) //pokud jsem nenasel funkci
            {
                nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
                sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
                tmpNode2 = TSreadSymbol(nazev);
                if(tmpNode2 == NULL) //pokud jsem NEnasel promennou
                {
                    fprintf(stderr, "Promenna: \"%s\" nebyla deklarovana\n", token.data);
                    return ESEM;
                }
            }
            else //nasel jsem funkci
            {
                tmpNode = tmpNode2;
            }
        }


        if (((tmpNode != NULL)) && ((tmpNode->data.varFc) == true))
        {
            volana_fce=tmpNode;
            cilovaAdresaFce=tmpNode;
            getToken(); //pokusim se nacist zavorku
            if(error != EOK)
                return error;

            if(token.stav != S_LZ) // "("
                return ESYN;

            bool bylWrite = isWrite; //pokud jsme ve write
            isWrite = false;

            error = args();

            isWrite =bylWrite;

            if(error != EOK)
                return error;

            if(isWrite == false)
            {
                if(token.stav != S_PZ) // ")"
                    return ESYN;
                getToken();
            }



            int kam=tmpNode->data.jump;
            ta_InsertJump(&ta,I_FCEJUMP,NULL, kam);

            if(error != EOK)
                return error;
            if(isWriteFunc == false)
            {
                if(token.stav != S_STREDNIK) // ";"
                    return ESYN;
            }


            ta_Insert(&ta, I_ASSIGN, cilovaAdresaFce, NULL, cilovaAdresa);
        }
        else //jinak je promenna nebo cislo ,str atd...
        {
            error = pparser();
            if(error != EOK)
                return error;

            ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazov), NULL, cilovaAdresa);
            if(error != EOK)
                return error;
            if(token.stav != S_STREDNIK) // ";"
                return ESYN;
        }
    }

    if(error != EOK)
        return error;

    return error;
}

tError args()
{
    getToken(); //pokusim se nacist ID
    if(error != EOK)
        return error;

    if(token.stav == S_PZ) // ")" //funkce nema zadne parametry
        return error;

    if(token.stav == S_IDENT) // ID
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            tBTSUzolPtr tmpNode = TSreadSymbol(token.data);
            if(tmpNode == NULL) //pokud jsem nenarazil na volani funkce
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else
            {
                isWriteFunc = true;
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                //dataStromu.typ = tDouble;

                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky


                tBTSUzolPtr tmpCil = cilovaAdresa;
                cilovaAdresa = TSreadSymbol(nazev);
                error = litExpr();
                ta_Insert(&ta, I_WRITE, cilovaAdresa,NULL, NULL);
                if(error != EOK)
                    return error;

                //ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, cilovaAdresa);
                cilovaAdresa = tmpCil;
                isWriteFunc = false;
                isWrite = true;
                getToken();
                if(error != EOK)
                    return error;

                //if(token.stav != S_CARKA) // ","
                // return ESYN;
            }
        }
        else if(isWrite == false) //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
            sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
            tBTSUzolPtr tmpNode = TSreadSymbol(nazev);
            if(tmpNode == NULL) //jmeno neni v tabulce
            {
                return ESEM;
            }
            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[0]=tmpNode;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_DOUBLE))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }

        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }

            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[0]=TSreadSymbol(nazev);
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_INT))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[0]=TSreadSymbol(nazev);
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_EXPO))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[0]=TSreadSymbol(nazev);
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
            }
        }
    }
    else if((token.stav == S_STR))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.s = token.data; //prevedu string na hodnotu
            dataStromu.typ = tString; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[0]=TSreadSymbol(nazev);
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
            }
        }
    }
    else if((token.stav == S_KLUC))
    {
        if((!strcmp(token.data, "true")) || (!strcmp(token.data, "false")))
        {
            if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                if(!(strcmp(token.data, "true")))
                    dataStromu.value.b = true; //prevedu string na hodnotu
                else dataStromu.value.b = false;
                dataStromu.typ = tBool; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isType || isFind || isSubs || isSort)
                {
                    pole_argumentu[0]=TSreadSymbol(nazev);
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
                }

            }
        }

        else if(!(strcmp(token.data, "nil")))
        {
            if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                dataStromu.value.d = 10;
                dataStromu.typ = tNil; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky
                if(isType || isFind || isSubs || isSort)
                {
                    pole_argumentu[0]=TSreadSymbol(nazev);
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
                }
            }
        }



    }
    else if((token.stav == S_SUB))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            getToken();
            if(error != EOK)
                return error;

            if((token.stav == S_DOUBLE) || (token.stav == S_INT) || (token.stav == S_EXPO))
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                dataStromu.value.d = (atof(token.data))*(-1); //prevedu string na hodnotu
                dataStromu.typ = tDouble; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
            else return ESEM;
        }
    }
    else if(token.stav == S_LZ) // ID
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else return ESYN;
    }

    else return ESEM;

    error = argsNext();
    if(error != EOK)
        return error;

    return error;
}

tError argsNext()
{
    if(isWrite == false) //pokud jsme ve write, tak token byl uz nacten v pparser()
        getToken();

    if(error != EOK)
        return error;

    if(token.stav == S_PZ) // ")" //funkce nema dalsi parametry
        return error;

    if(token.stav != S_CARKA) // "," //ted musi byt carka
        return ESYN;      //<params_next>	->	"," ID <params_next>

    getToken();
    if(error != EOK)
        return error;

    if(token.stav == S_IDENT) // ID
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            tBTSUzolPtr tmpNode = TSreadSymbol(token.data);
            if(tmpNode == NULL) //pokud jsem nenarazil na volani funkce
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else
            {
                isWriteFunc = true;
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                //dataStromu.typ = tDouble;

                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky


                tBTSUzolPtr tmpCil = cilovaAdresa;
                cilovaAdresa = TSreadSymbol(nazev);
                error = litExpr();
                ta_Insert(&ta, I_WRITE, cilovaAdresa,NULL, NULL);
                if(error != EOK)
                    return error;

                //ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, cilovaAdresa);
                cilovaAdresa = tmpCil;
                isWriteFunc = false;
                isWrite = true;
                getToken();
                if(error != EOK)
                    return error;

                //if(token.stav != S_CARKA) // ","
                // return ESYN;
            }
        }
        else if (isWrite == false) //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
            sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
            tBTSUzolPtr tmpNode = TSreadSymbol(nazev);
            if(tmpNode == NULL) //jmeno neni v tabulce
            {
                return ESEM;
            }

            if(isType || isFind || isSubs || isSort)
            {
                if(((*cislo_argumentu)%3)==0)
                {
                    pole_argumentu[0]=(tBTSUzolPtr) advRealloc( pole_argumentu, sizeof(tBTSUzolPtr)*(*cislo_argumentu+3));
                }
                pole_argumentu[(*cislo_argumentu)]=tmpNode;
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_DOUBLE))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_INT))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_EXPO))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_STR))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazov = nazev;
            dataStromu.value.s = token.data; //prevedu string na hodnotu
            dataStromu.typ = tString; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isType || isFind || isSubs || isSort)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == S_KLUC))
    {
        if((!strcmp(token.data, "true")) || (!strcmp(token.data, "false")))
        {
            if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                if(!(strcmp(token.data, "true")))
                    dataStromu.value.b = true; //prevedu string na hodnotu
                else dataStromu.value.b = false;
                dataStromu.typ = tBool; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isType || isFind || isSubs || isSort)
                {
                    pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                    (*cislo_argumentu)++;
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

                }

            }
        }

        else if((!strcmp(token.data, "nil")))
        {
            if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
            }
            else
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                dataStromu.value.d = 10;
                dataStromu.typ = tNil; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky
                if(isType || isFind || isSubs || isSort)
                {
                    pole_argumentu[0]=TSreadSymbol(nazev);
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
                }
            }
        }


    }
    else if((token.stav == S_SUB))
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            getToken();
            if(error != EOK)
                return error;

            if((token.stav == S_DOUBLE) || (token.stav == S_INT) || (token.stav == S_EXPO))
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazov = nazev;
                dataStromu.value.d = (atof(token.data))*(-1); //prevedu string na hodnotu
                dataStromu.typ = tDouble; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isType || isFind || isSubs || isSort)
                {
                    pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                    (*cislo_argumentu)++;
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

                }

            }
            else return ESEM;
        }
    }
    else if(token.stav == S_LZ) // ID
    {
        if(isWrite) //pokud potrebuji zavolat vyhodnoceni vyrazu u write()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_WRITE, TSreadSymbol(neterm.data.nazov),NULL, NULL);
        }
        else return ESYN;
    }
    else return ESEM;

    error = argsNext();
    if(error != EOK)
        return error;

    return error;
}


/**
 * <params>		->	ID <params_next>
 * <params>		->	EPS
 */
tError params()
{
    getToken();
    functionNodeTmp->data.argCounter = 0;
    if(error != EOK)
        return error;

    if(token.stav == S_PZ) // ")" //funkce nema parametry
        return error; //<params>	->	EPS

    if(token.stav == S_IDENT) //<params>	->	ID <params_next>
    {
        if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
            return ESEM;

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
        dataStromu.nazov = nazev;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNodeTmp->data.argCounter = functionNodeTmp->data.argCounter + 1; //prictu dalsi argument
        functionNode->data.nextNode= TSreadSymbol(nazev);
        functionNode = TSreadSymbol(nazev);
        functionNode->data.nextNode = NULL;

        error = paramsNext(); //<params_next>
        if(error != EOK)
            return error;
    }

    return error;
}

/**
 * <params_next>	->	EPS
 * <params_next>	->	"," ID <params_next>
 */
tError paramsNext()
{
    getToken();
    if(error != EOK)
        return error;

    if(token.stav == S_PZ) // ")" //funkce nema dalsi parametry
        return error;

    if(token.stav != S_CARKA) // "," //ted musi byt carka
        return ESYN;      //<params_next>	->	"," ID <params_next>

    getToken(); //nacti ID
    if(error != EOK)
        return error;
    if(token.stav == S_IDENT) //ID
    {
        if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
            return ESEM;

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
        dataStromu.nazov = nazev;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNodeTmp->data.argCounter = functionNodeTmp->data.argCounter + 1; //prictu dalsi argument
        functionNode->data.nextNode = TSreadSymbol(nazev);
        functionNode = TSreadSymbol(nazev);
        functionNode->data.nextNode = NULL;

        error = paramsNext(); //zavolej se zase
    }
    else return ESYN; //za carkou musi byt ID

    return error;
}

void najdiKolize(tBTSUzolPtr uzel)
{
    if(uzel == NULL)
        return;
    if(error != EOK)
        return ;

    if((uzel->kluc[0] != '#') && (uzel->kluc[0] != '@')) //pokud jsem nasel uzel s funkci
    {
        char *nazev = advMalloc(strlen(uzel->kluc)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", uzel->kluc, token.data); //vlozim string do promenne
        if((TSreadSymbol(nazev)) != NULL) //promenna uz existuje
            error = ESEM;
        else
        {
            if(uzel->lptr != NULL)
                najdiKolize(uzel->lptr);
            if(error != EOK)
                return ;

            if(uzel->rptr != NULL)
                najdiKolize(uzel->rptr);
        }
    }
}
