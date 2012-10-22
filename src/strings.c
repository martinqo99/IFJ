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
#define SIZE 20 // zacatecni velikost pole, pri realokaci se alokuje +SIZE
#define TRUE 1
#define FALSE 0

/**
 * @info      Inicializace struktury pro string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strInit (tString *str)
{
  str->lenght = 0;

  str->data = (char *) mmuCalloc(SIZE, sizeof(char));
  if (str->data == NULL)
    return FALSE;
  str->allocated = SIZE;

  return TRUE;
}

/**
 * @info      Uvolni string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strFree (tString *str)
{
  if (str->data)
    mmuFree(str->data);
  str->data = NULL;
  str->allocated = 0;
  str->lenght = 0;

  return TRUE;
}

/**
 * @info      Smaze pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strClear (tString *str)
{
  uint i = str->lenght;
  while (i--)
    str->data[i] = '\0';
  str->lenght = 0;

  return TRUE;
}

/**
 * @info      Prida znak na konec retezce
 * @param   tString - struktura s polem a jeho rozmery
 * @param   char - pridavany znak
 * @return  TRUE || FALSE
 */
int strAdd (tString *str, char x)
{
  if (str->allocated - 2 == str->lenght) {
    str->allocated += SIZE;
    str->data = (char *) mmuRealloc(str->data, str->allocated * sizeof(char));
    if (str->data == NULL)
      return FALSE;
  }

  str->data[str->lenght++] = x;
  str->data[str->lenght] = '\0';

  return TRUE;
}

/**
 * @info      Zkopiruje pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strCopy (tString *str, char *array)
{
  array = (char *) mmuCalloc(str->lenght + 1, sizeof(char));
  if (array == NULL)
    return FALSE;

  int i = str->lenght;
  do
    array[i] = str->data[i];
  while (i--);

  return TRUE;
}

/**
 * @info      Zkopiruje celou strukturu
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strCopyString (tString *strl, tString *strr)
{
  strr->data = (char *) mmuCalloc(strl->allocated, sizeof(char));
  if (strr->data == NULL)
    return FALSE;
  strr->lenght = strl->lenght;
  strr->allocated = strl->allocated;

  int i = strr->lenght;
  do
    strr->data[i] = strl->data[i];
  while (i--);

  return TRUE;
}

/**
 * @info      Porovna dva stringy
 * @param   tString - struktura s polem a jeho rozmery
 * @return  TRUE || FALSE
 */
int strCmp (tString *strl, tString *strr)
{
  if (strl->allocated != strr->allocated)
    return FALSE;
  if (strl->lenght != strr->lenght)
    return FALSE;
  if (strcmp(strl->data, strr->data) != 0)
    return FALSE;

  return TRUE;
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
  return str->lenght;
}

/**
 * @info      Vrati naalokovanou velikost
 * @param   tString - struktura s polem a jeho rozmery
 * @return  naalokovana velikost
 */
uint strSize (tString *str)
{
  return str->allocated;
}
