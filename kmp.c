/**
 * Predmet:   IFJ / IAL
 * Projekt:   Implementace interpretu imperativniho jazyka IFJ12
 * Varianta:  a/1/I
 * Soubor:    kmp.c
 * Popis:     Hledani podretezce v retezci pomoci Knuth-Moris-Prattova algoritmu
 * Datum:     17.11.2012
 * Autori:    Frantisek Kolacek   <xkolac12@stud.fit.vutbr.cz>
 *            Petr Matyas         <xmatya03@stud.fit.vutbr.cz>
 *            Veronika Necasova   <xnecas21@stud.fit.vutbr.cz>
 *            Michaela Muzikarova <xmuzik04@stud.fit.vutbr.cz>
 *            Dalibor Skacel      <xskace11@stud.fit.vutbr.cz>
 */

#include "kmp.h"

/*
algorithm kmp_table:
    input:
        an array of characters, W (the word to be analyzed)
        an array of integers, T (the table to be filled)
    output:
        nothing (but during operation, it populates the table)

    define variables:
        an integer, pos ← 2 (the current position we are computing in T)
        an integer, cnd ← 0 (the zero-based index in W of the next
character of the current candidate substring)

    (the first few values are fixed but different from what the algorithm
might suggest)
    let T[0] ← -1, T[1] ← 0

    while pos is less than the length of W, do:
        (first case: the substring continues)
        if W[pos - 1] = W[cnd],
          let cnd ← cnd + 1, T[pos] ← cnd, pos ← pos + 1

        (second case: it doesn't, but we can fall back)
        otherwise, if cnd > 0, let cnd ← T[cnd]

        (third case: we have run out of candidates.  Note cnd = 0)
        otherwise, let T[pos] ← 0, pos ← pos + 1
*/
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

/*
  algorithm kmp_search:
    input:
        an array of characters, S (the text to be searched)
        an array of characters, W (the word sought)
    output:
        an integer (the zero-based position in S at which W is found)

    define variables:
        an integer, m ← 0 (the beginning of the current match in S)
        an integer, i ← 0 (the position of the current character in W)
        an array of integers, T (the table, computed elsewhere)

    while m+i is less than the length of S, do:
        if W[i] = S[m + i],
            if i equals the (length of W)-1,
                return m
            let i ← i + 1
        otherwise,
            let m ← m + i - T[i],
            if T[i] is greater than -1,
                let i ← T[i]
            else
                let i ← 0

    (if we reach here, we have searched all of S unsuccessfully)
    return the length of S
*/
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

    //for(unsigned int i = 0; i < text.len; i++)
    //    printf("Polozka %d: %d\n", i, table[i]);

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

    return text.len;
}

/*
int kmpSearch (tString text, tString searched)
{
  if (text.len == 0)
    return text.len; // neni v cem hledat


  int match = 0, index = 0, table[text.len];
  printf("DEBUG: %u\n", text.len);
  //kmpCrtTable (text, &table[text.len]); // vytvoreni pomocne tabulky
  printf("DEBUG: %u\n", text.len);
  printf("\ntext.len = %u", text.len);
  printf("\nTabulka\n");
  for (unsigned int i = 0; i < text.len; i++)
    printf("Polozka %d: %d\n", i, table[i]);
  printf("Konec tabulky\n\n");

  while ((match+index) < (int) text.len) {
    if (searched.data[index] == text.data[match + index]) {
      if (index == (int) searched.len - 1)
        return match; // stejny znak a zaroven posledni hledany => uspech, konec
      index++; // stejny znak
    }
    else {
      match += index - table[index]; // znak je jiny => posun dale
      if (table[index] > -1)
        index = table[index];
      else
        index = 0; // KMP magic
    }
  }

  return text.len; // hledany text tu neni
}
*/
