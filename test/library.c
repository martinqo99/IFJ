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
E_CODE input (tSymbolData *dest)
{
  int c;
  tString save;
  E_CODE err = strInit(&save);
  while ((c = fgetc(stdin)) != EOF && c != '\n' && err == ERROR_OK)
    err = strAdd(&save, (char) c);

  dest->type = DT_STRING;
  dest->data.sData = save;
  return err;
}

/**
 * @info      Fce prevede string nebo cislo na cislo
 * @param   tSymbolData - string, cislo nebo bool
 * @param   E_CODE - odkaz a chybovy kod
 * @return  tSymbolData - vrati prevedene cislo
 */
E_CODE numeric (tSymbolData *dest, tSymbolData *id)
{
  if (id == NULL) return ERROR_OK;
  if (id->type == DT_BOOL || id->type == DT_NIL || id->type == DT_UNKNOWN) {
    dest->type = DT_NIL;
    return ERROR_NUMERIC_CONVERSION;
  }

  if (id->type == DT_NUMBER) {
    dest->type = DT_NUMBER;
    dest->data.dData = id->data.dData;
    return ERROR_OK;
  }

  char *endptr = NULL;
  dest->type = DT_NUMBER;
  dest->data.dData = strtod(id->data.sData.data, &endptr);

  if (*endptr != '\0' || strcmp(endptr, id->data.sData.data) == 0) {
    dest->type = DT_NIL;
    return ERROR_NUMERIC_CONVERSION;
  }

  return ERROR_OK;
}

/**
 * @info      Fce vytiskne vsechno co prislo na vstup
 * @param   tSymbolData - string, cislo nebo bool; nahodny pocet
 * @return  tSymbolData - vrati DT_NIL
 */
void print (tSymbolData id)
{
  if (id.type == DT_NIL)
    printf("Nil");
  else if (id.type == DT_BOOL)
    if (id.data.bData == false)
      printf("false");
    else printf("true");
  else if (id.type == DT_NUMBER)
    printf("%g", id.data.dData);
  else if (id.type == DT_STRING)
    printf ("%s", id.data.sData.data);
}

/**
 * @info      Fce vrati ciselny identifikator datoveho typu promenne
 * @param   tSymbolData - string, cislo nebo bool
 * @return  tSymbolData - vrati identifikator promenne
 */
void typeOf (tSymbolData *dest, tSymbolData *id)
{
  if (id == NULL) return;
  dest->type = DT_NUMBER;

  if (id->type == DT_NIL)
    dest->data.dData = 0.0;
  else if (id->type == DT_BOOL)
    dest->data.dData = 1.0;
  else if (id->type == DT_NUMBER)
    dest->data.dData = 3.0;
  else if (id->type == DT_FUNCTION)
    dest->data.dData = 6.0;
  else
    dest->data.dData = 8.0;
}

/**
 * @info      Fce vrati delku retezce, jinak vrati 0.0
 * @param   tSymbolData - string, cislo nebo bool
 * @return  tSymbolData - vrati delku retezce
 */
void len (tSymbolData *dest, tSymbolData *id)
{
  if (id == NULL) return;
  if (id->type == DT_STRING) {
    dest->type = DT_NUMBER;
    dest->data.dData = (double) strlen(id->data.sData.data);
  }
  else {
    dest->type = DT_NUMBER;
    dest->data.dData = 0.0;
  }
}

/**
 * @info      Fce hleda podretezec v retezci
 * @param   tSymbolData - retezce, v prvnim se hleda, druhy se hleda v prvnim
 * @return  tSymbolData - vrati pozici v prvnim stringu, na ktere nasel druhy string
 */
void find (tSymbolData *dest, tSymbolData *text, tSymbolData *searched)
{
  if (text == NULL || searched == NULL) return;
  dest->type = DT_NUMBER;
  dest->data.dData = (double) kmpSearch(text->data.sData, searched->data.sData);
}

/**
 * @info      Fce seradi hodnoty ve stringu
 * @param   tSymbolData - retezec k serazeni
 * @return  tSymbolData - vrati serazeny string
 */
E_CODE sort (tSymbolData *dest, tSymbolData *text)
{
  if (text == NULL) return ERROR_OK;
  if (text->type != DT_STRING) {
    dest->type = DT_NIL;
    return ERROR_INCOMPATIBLE_TYPE;
  }
  else
    dest->data.sData = strCreate(text->data.sData.data);
    quicksort(&dest->data.sData.data, 0, text->data.sData.len);
  return ERROR_OK;
}
