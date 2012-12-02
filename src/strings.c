/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    strings.c
 * Popis:     Diky tomuto souboru muzeme pracovat se stringy :P
 * Datum:     17.10.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "strings.h"

/**
 * @info      Inicializace struktury pro string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  E_CODE - chybovy kod
 */
E_CODE strInit (tString *str)
{
  str->len = 0;

  str->data = (char *) mmuCalloc(SIZE, sizeof(char));
  if (str->data == NULL)
    return ERROR_COMPILATOR;
  str->alloc = SIZE;

  return ERROR_OK;
}

/**
 * @info      Porovnani struktury s retezcem
 * @param   tString - struktura s polem a jeho rozmery
 * @param   const char - pole znaku se kterym porovnavame
 * @return  TRUE || FALSE
 */
/*int strCmpRaw (tString *str, const char x)
{
  if (strcmp(str->data, x) == 0)
    return TRUE;
  return FALSE;
}*/

/**
 * @info      Vytvori strukturu a prida do ni *char z parametru
 * @param   char* - pole znaku, nad kterym vytvorime novou strukturu string
 * @return  tString - Struktura s polem a jeho rozmery
 */
tString strCreate (const char *array)
{
  tString str = {NULL, 0, 0};
  if (strInit(&str) == ERROR_COMPILATOR)
    return str;

  int i = 0;
  while (array[i] != '\0')
    if (strAdd(&str, array[i++]) == ERROR_COMPILATOR) {
      strFree(&str);
      return str;
    }

  return str;
}

/**
 * @info      Uvolni string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  E_CODE - chybovy kod
 */
E_CODE strFree (tString *str)
{
  if (str->data)
    mmuFree(str->data);
  str->data = NULL;
  str->alloc = 0;
  str->len = 0;

  return ERROR_OK;
}

/**
 * @info      Smaze pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  E_CODE - chybovy kod
 */
E_CODE strClear (tString *str)
{
  uint i = str->len;
  while (i--)
    str->data[i] = '\0';
  str->len = 0;

  return ERROR_OK;
}

/**
 * @info      Prida znak na konec retezce
 * @param   tString - struktura s polem a jeho rozmery
 * @param   char - pridavany znak
 * @return  E_CODE - chybovy kod
 */
E_CODE strAdd (tString *str, char x)
{
  if (str->alloc - 2 == str->len) {
    str->alloc += SIZE;
    str->data = (char *) mmuRealloc(str->data, str->alloc * sizeof(char));
    if (str->data == NULL)
      return ERROR_COMPILATOR;
  }

  str->data[str->len++] = x;
  str->data[str->len] = '\0';

  return ERROR_OK;
}

/**
 * @info      Zkopiruje pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  E_CODE - chybovy kod
 */
E_CODE strCopy (tString *str, char *array)
{
  array = (char *) mmuCalloc(str->len + 1, sizeof(char));
  if (array == NULL)
    return ERROR_COMPILATOR;

  int i = str->len;
  do
    array[i] = str->data[i];
  while (i--);

  return ERROR_OK;
}

/**
 * @info      Zkopiruje celou strukturu
 * @param   tString - struktura s polem a jeho rozmery
 * @return  E_CODE - chybovy kod
 */
E_CODE strCopyString (tString *strl, tString *strr)
{
  strr->data = (char *) mmuCalloc(strl->alloc, sizeof(char));
  if (strr->data == NULL)
    return ERROR_COMPILATOR;
  strr->len = strl->len;
  strr->alloc = strl->alloc;

  /*
  int i = strr->len;
  do
    strr->data[i] = strl->data[i];
  while (i--);
  */
  for(unsigned int i = 0; i < strr->len; i++)
        strr->data[i] = strl->data[i];

  return ERROR_OK;
}

/**
 * @info      Porovna dva stringy
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE 1 prvni je delsi || FALSE 0 stejne || NEGATIVE -1 prvni je kratsi
 */
int strCmp (tString *strl, tString *strr)
{
  if (strl->len < strr->len)
    return -1; // prvni je kratsi
  else if (strl->len > strr->len)
    return 1; // prvni je delsi

  return strcmp(strl->data,strr->data);
}

/*
 * @info      Porovnani struktury s retezcem
 * @param   tString - struktura s polem a jeho rozmery
 * @param   char - pole znaku se kterym porovnavame
 * @return  TRUE || FALSE
 */
int strCmpRaw (tString *strl, const char *strr)
{
  return strcmp(strl->data,strr);
}

/**
 * @info      Vrati ukazatel na pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  ukazatel na vstupni string
 */
char *strRaw (tString *str)
{
  return str->data;
}

/**
 * @info      Vrati delku stringu
 * @param   tString - struktura s polem a jeho rozmery
 * @return  delka stringu
 */
uint strLen (tString *str)
{
  return str->len;
}

/**
 * @info      Vrati naalokovanou velikost
 * @param   tString - struktura s polem a jeho rozmery
 * @return  naalokovana velikost
 */
uint strSize (tString *str)
{
  return str->alloc;
}
