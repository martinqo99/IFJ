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
 * @return  tSymbolData - nacteny retezec
 */
tSymbolData input (E_CODE *err)
{
  int c;
  tString save;
  *err = strInit(&save);
  while ((c = fgetc(stdin)) != EOL && c != '\n' && *err == ERROR_OK)
    *err = strAdd(&save, (char) c);

  tSymbolData tmp;
  tmp.type = DT_STRING;
  tmp.data.sData = save;
  return tmp;
}

/**
 * @info      Fce prevede string nebo cislo na cislo
 * @param   tSymbolData - string, cislo nebo bool
 * @param   E_CODE - odkaz a chybovy kod
 * @return  tSymbolData - vrati prevedene cislo
 */
tSymbolData numeric (tSymbolData id, E_CODE *err)
{
  tSymbolData tmp;
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
 * @param   tSymbolData - string, cislo nebo bool; nahodny pocet
 * @return  tSymbolData - vrati DT_NIL
 */
tSymbolData print (tSymbolData id1, ...)
{
  va_list ap;
  tSymbolData i;

  va_start(ap, id1);
  for (i = id1; ; i = va_arg(ap, tSymbolData))
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

  tSymbolData tmp;
  tmp.type = DT_NIL;
  return tmp;
}

/**
 * @info      Fce vrati ciselny identifikator datoveho typu promenne
 * @param   tSymbolData - string, cislo nebo bool
 * @return  tSymbolData - vrati identifikator promenne
 */
tSymbolData typeOf (tSymbolData id)
{
  tSymbolData tmp;
  tmp.type = DT_NUMBER;

  if (id.type == DT_NIL)
    tmp.data.dData = 0.0;
  else if (id.type == DT_BOOL)
    tmp.data.dData = 1.0;
  else if (id.type == DT_NUMBER)
    tmp.data.dData = 3.0;
  else if (id.type == DT_FUNCTION)
    tmp.data.dData = 6.0;
  else
    tmp.data.dData = 8.0;

  return tmp;
}

/**
 * @info      Fce vrati delku retezce, jinak vrati 0.0
 * @param   tSymbolData - string, cislo nebo bool
 * @return  tSymbolData - vrati delku retezce
 */
tSymbolData len (tSymbolData id)
{
  tSymbolData tmp;
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
 * @param   tSymbolData - retezce, v prvnim se hleda, druhy se hleda v prvnim
 * @return  tSymbolData - vrati pozici v prvnim stringu, na ktere nasel druhy string
 */
tSymbolData find (tSymbolData text, tSymbolData searched)
{
  tSymbolData tmp;
  tmp.type = DT_NUMBER;
  tmp.data.dData = (double) kmpSearch(text.data.sData, searched.data.sData)
  return tmp;
}

/**
 * @info      Fce seradi hodnoty ve stringu
 * @param   tSymbolData - retezec k serazeni
 * @return  tSymbolData - vrati serazeny string
 */
tSymbolData sort (tSymbolData nonsorted, E_CODE *err)
{
  if (nonsorted.type != DT_STRING) {
    *err = ERROR_INCOMPATIBLE_TYPE;
    return {.type = DT_NIL};
  }
  quicksort(&nonsorted.data.sData.data, 0, nonsorted.data.sData.len-1);
  return nonsorted;
}
