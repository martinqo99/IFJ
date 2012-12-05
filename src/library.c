/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    library.c
 * Popis:     Vestavene funkce
 * Datum:     3.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "library.h"

/**
 * @info      Fce ziska vstup od uzivatele
 * @param   E_CODE - odkaz na chybovy kod
 * @return  tLibraryData - nacteny retezec
 */
tLibraryData input (E_CODE *err)
{
  int c;
  tString save;
  *err = strInit(&save);
  while ((c = fgetc(stdin)) != EOL && c != '\n' && *err == ERROR_OK)
    *err = strAdd(&save, (char) c);

  tLibraryData tmp;
  tmp.type = DT_STRING;
  tmp.data.sData = save;
  return tmp;
}

/**
 * @info      Fce prevede string nebo cislo na cislo
 * @param   tLibraryData - string, cislo nebo bool
 * @param   E_CODE - odkaz a chybovy kod
 * @return  tLibraryData - vrati prevedene cislo
 */
tLibraryData numeric (tLibraryData id, E_CODE *err)
{
  tLibraryData tmp;
  if (id.type == DT_BOOL || id.type == DT_NIL || id.type == DT_UNKNOWN) {
    *err = ERROR_NUMERIC_CONVERSION;
    tmp.type = DT_NIL;
    return tmp;
  }

  if (id.type == DT_NUMBER) {
    tmp.type = DT_NUMBER;
    tmp.data.dData = id.data.dData;
    return tmp;
  }

  char *endptr = NULL;
  tmp.type = DT_NUMBER;
  tmp.data.dData = strtod(id.data.sData.data, &endptr);

  if (*endptr != '\0' || strcmp(endptr, id.data.sData.data) == 0) {
    *err = ERROR_NUMERIC_CONVERSION;
    tmp.type = DT_NIL;
    return tmp;
  }

  return tmp;
}

/**
 * @info      Fce vytiskne vsechno co prislo na vstup
 * @param   tLibraryData - string, cislo nebo bool; nahodny pocet
 * @return  tLibraryData - vrati DT_NIL
 */
tLibraryData print (tLibraryData id1, ...)
{
  va_list ap;
  tLibraryData i;

  va_start(ap, id1);
  for (i = id1; ; i = va_arg(ap, tLibraryData))
    if (i.type == DT_NIL)
      printf("Nil");
    else if (i.type == DT_BOOL)
      if (i.data.iData == 0)
        printf("false");
      else printf("true");
    else if (i.type == DT_NUMBER)
      printf("%g", i.data.dData);
    else if (i.type == DT_STRING)
      printf ("%s", i.data.sData.data);
    else if (i.type == DT_UNKNOWN)
      break;
  va_end(ap);

  tLibraryData tmp;
  tmp.type = DT_NIL;
  return tmp;
}

/**
 * @info      Fce vrati ciselny identifikator datoveho typu promenne
 * @param   tLibraryData - string, cislo nebo bool
 * @return  tLibraryData - vrati identifikator promenne
 */
tLibraryData typeOf (tLibraryData id)
{
  tLibraryData tmp;
  tmp.type = DT_NUMBER;

  if (id.type == DT_NIL)
    tmp.data.dData = 0.0;
  else if (id.type == DT_BOOL)
    tmp.data.dData = 1.0;
  else if (id.type == DT_NUMBER)
    tmp.data.dData = 3.0;
  else if (id.type == DT_UNKNOWN)
    tmp.data.dData = 6.0;
  else
    tmp.data.dData = 8.0;

  return tmp;
}

/**
 * @info      Fce vrati delku retezce, jinak vrati 0.0
 * @param   tLibraryData - string, cislo nebo bool
 * @return  tLibraryData - vrati delku retezce
 */
tLibraryData len (tLibraryData id)
{
  tLibraryData tmp;
  if (id.type == DT_STRING) {
    tmp.type = DT_NUMBER
    tmp.data.dData = (double) strlen(id.data.sData.data);
  else {
    tmp.type = DT_NUMBER;
    tmp.data.dData = 0.0;
  }

  return tmp;
}

/**
 * @info      Fce hleda podretezec v retezci
 * @param   tLibraryData - retezce, v prvnim se hleda, druhy se hleda v prvnim
 * @return  tLibraryData - vrati pozici v prvnim stringu, na ktere nasel druhy string
 */
tLibraryData find (tLibraryData text, tLibraryData searched)
{
  tLibraryData tmp;
  tmp.type = DT_NUMBER;
  tmp.data.dData = (double) kmpSearch(text.data.sData, searched.data.sData)
  return tmp;
}

/**
 * @info      Fce seradi hodnoty ve stringu
 * @param   tLibraryData - retezec k serazeni
 * @return  tLibraryData - vrati serazeny string
 */
tLibraryData sort (tLibraryData nonsorted)
{
  // return to_co_dela_verca;
}
