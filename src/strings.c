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
#include "MMU.c"
#define SIZE 20 // zacatecni velikost pole, pri realokaci se alokuje +SIZE

/**
 * @info      Inicializace struktury pro string
 * @param   tString - struktura s polem a jeho rozmery
 * @return  EXIT_SUCCESS || EXIT_FAILURE
 */
int strInit (tString *str)
{
  str->lenght = 0;
  str->allocated = SIZE;

  str->data = (char *) mmuCalloc(str->allocated, sizeof(char));
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
  mmuFree(str->data);
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
  while (i--)
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
  if (str->allocated - 1 == str->lenght) {
    str->allocated += SIZE;
    str->data = (char *) mmuRealloc(str->data, str->allocated * sizeof(char));
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
  array = (char *) mmuCalloc(str->lenght + 1, sizeof(char));
  if (array == NULL)
    return EXIT_FAILURE;

  int i = str->lenght;
  do
    array[i] = str->data[i];
  while (i--);

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

  strr->data = (char *) mmuCalloc(strr->allocated, sizeof(char));
  if (strr->data == NULL)
    return EXIT_FAILURE;

  int i = strr->lenght;
  do
    strr->data[i] = strl->data[i];
  while (i--);

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