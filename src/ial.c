/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    ial.c
 * Popis:     Soubor s fcemi do IAL
 * Datum:     8.12.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "strings.h"
#include "mmu.h"

///////////////////////// Knuth-Moris-Prattuv algoritmus ///////////////////////
/**
 * @info      Vytvoreni pomocne tabulky
 * @param   tString - struktura s polem a jeho rozmery
 * @return  nic
 */

void kmpCrtTable (tString text, int* table){
  int position = 2, index = 0;

  table[0] = -1;
  table[1] = 0; // fixni hodnoty

  while (position < (int) text.len)
    if (text.data[position - 1] == text.data[index]) {
      index++;
      table[position] = index;
      position++; // KMP magic
    }
    else if (index > 0)
      index = table[index]; // KMP magic
    else {
      table[position] = 0;
      position++; // KMP magic
    }
}

/**
 * @info      Hledani podretezce v retezci
 * @param   tString - struktura s polem a jeho rozmery
 * @return  pozice prvniho stejneho znaku pri shode, jinak delka textu, ve kterem hledame
 */

int kmpSearch(tString text, tString searched){
    if(text.len == 0)
        return 0;

    int match = 0;
    int index = 0;
    int table[text.len];

    kmpCrtTable(text, table);

    while((match + index) < (int) text.len){
        if(searched.data[index] == text.data[match + index]){
            if(index == (int)searched.len - 1)
                return match;

            index++;
        }
        else{
            match += index - table[index];

            if(table[index] > -1)
                index = table[index];
            else
                index = 0;
        }
    }

    return -1;
}

/////////////////////// Quicksort //////////////////////////////////////////////

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

////////////////// Tabulka symbolu pomoci binarniho stromu /////////////////////
