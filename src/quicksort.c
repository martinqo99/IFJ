/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    quicksort.c
 * Popis:     Fce pro quicksort
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "quicksort.h"

/**
 * @info      Radici fce quicksort
 * @param   char* - pole znaku
 * @param   int - zacatek a konec
 * @return  void
 */
void quicksort(char *array[], int left_begin, int right_begin)
{
  int pivot = (*array)[(left_begin + right_begin) / 2];
  int left_index = left_begin, right_index = right_begin, pom;
  do {
    while ((*array)[left_index] < pivot && left_index < right_begin)
      left_index++;
    while ((*array)[right_index] > pivot && right_index > left_begin)
      right_index--;

    if (left_index <= right_index) {
      pom = (*array)[left_index];
      (*array)[left_index] = (*array)[right_index];
      (*array)[right_index] = pom;
      if (left_index < right_begin)
        left_index++;
      if (right_index > left_begin)
        right_index--;
    }
  } while (left_index < right_index);
  if (right_index > left_begin) quicksort(array, left_begin, right_index);
  if (left_index < right_begin) quicksort(array, left_index, right_begin);
}
