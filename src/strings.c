/*
 * Predmet: IFJ / IAL
 * Projekt: Implementace interpretu imperativniho jazyka IFJ12
 * Soubor:  strings.c
 * Datum:   17.10.2012
 * Autori:  Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>,
 *          Petr Matyas         <xmatya03@stud.fit.vutbr.cz>,
 *          Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>,
 *          Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>,
 *          Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "strings.h"
#define SIZE 20 // zacatecni velikost pole, pri realokaci se zdvojnasobi

/**
 * @info      Inicializace struktury pro string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strInit (tString *str)
{
  str->lenght = 0;
  str->allocated = SIZE;

  str->data = (char *) calloc(str->allocated, sizeof(char));
  if (str->data == NULL)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

/**
 * @info      Uvolni string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strFree (tString *str)
{
  free(str->data);
  str->data = NULL;
  str->allocated = 0;
  str->lenght = 0;

  return EXIT_SUCCESS;
}

/**
 * @info      Smaze pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strClear (tString *str)
{
  uint i = str->lenght;
  while (--i)
    str->data[i] = '\0';
  str->lenght = 0;

  return EXIT_SUCCESS;
}

/**
 * @info      Prida znak na konec retezce
 * @param   tString - struktura s polem a jeho rozmery
 * @param   char - pridavany znak
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strAdd (tString *str, char x)
{
  if (str->allocated == str->lenght - 1) {
    str->allocated += SIZE;
    str->data = (char *) realloc(str->data, str->allocated * sizeof(char));
    if (str->data == NULL)
      return EXIT_FAILURE;
  }

  str->data[str->lenght++] = x;
  str->data[str->lenght] = '\0';

  return EXIT_SUCCESS;
}

/**
 * @info      Zkopiruje pole znaku
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strCopy (tString *str, char *array)
{
  array = (char *) calloc(str->lenght, sizeof(char));
  if (array == NULL)
    return EXIT_FAILURE;

  int i = str->lenght;
  while (i--)
    array[i] = str->data[i];

  return EXIT_SUCCESS;
}

/**
 * @info      Zkopiruje celou strukturu
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strCopyString (tString *strl, tString *strr)
{
  strr->lenght = strl->lenght;
  strr->allocated = strl->allocated;

  strr->data = (char *) calloc(strr->lenght, sizeof(char));
  if (strr->data == NULL)
    return EXIT_FAILURE;

  int i = strr->lenght;
  while (i--)
    strr->data[i] = strl->data[i];

  return EXIT_SUCCESS;
}

/**
 * @info      Porovna dva stringy
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strCmp (tString *strl, tString *strr)
{
  if (strl->allocated != strr->allocated)
    return EXIT_FAILURE;
  if (strl->lenght != strr->lenght)
    return EXIT_FAILURE;
  if (strcmp(strl->data, strr->data) != 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
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
int strLen (tString *str)
{
  return str->lenght;
}

/**
 * @info      Vrati naalokovanou velikost
 * @param   tString - struktura s polem a jeho rozmery
 * @return  naalokovana velikost
 */
int strSize (tString *str)
{
  return str->allocated;
}