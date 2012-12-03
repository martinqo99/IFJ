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
 * @return  tString - nacteny retezec
 */
tString input ()
{
  int c;
  tString save;
  E_CODE err = strInit(&save);
  while ((c = fgetc(stdin)) != EOL && c != '\n' && err == ERROR_OK)
    err = strAdd(&save, (char) c);

  return save;
}

/**
 * @info      Fce prevede string nebo cislo na cislo
 * @param   tLibraryData - string, cislo nebo bool
 * @return  double - vrati prevedene cislo
 */
double numeric (tLibraryData id)
{
  // nejaka black magic
}

/**
 * @info      Fce vytiskne vsechno co prislo na vstup
 * @param   tLibraryData - string, cislo nebo bool; nahodny pocet
 * @return  tKeyword - vrati nil
 */
tKeyword print (tLibraryData id1, ...)
{
  return KW_NIL;
}

/**
 * @info      Fce vrati ciselny identifikator datoveho typu promenne
 * @param   tLibraryData - string, cislo nebo bool
 * @return  double - vrati identifikator promenne
 */
double typeOf (tLibraryData id)
{

}

/**
 * @info      Fce vrati delku retezce, jinak vrati 0.0
 * @param   tLibraryData - string, cislo nebo bool
 * @return  double - vrati delku retezce
 */
double len (tLibraryData id)
{

}

/**
 * @info      Fce hleda podretezec v retezci
 * @param   tString - retezce, v prvnim se hleda, druhy se hleda v prvnim
 * @return  double - vrati pozici v prvnim stringu, na ktere nasel druhy string
 */
double find (tString text, tString searched)
{
  return (double) kmpSearch(text, searched);
}

/**
 * @info      Fce seradi hodnoty ve stringu
 * @param   tString - retezec k serazeni
 * @return  tString - vrati serazeny string
 */
tString sort (tString nonsorted)
{
  // return to_co_dela_verca;
}
