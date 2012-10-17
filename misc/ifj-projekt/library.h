/*
 * @description   Knihova vestavìných funkcí
 * @author        Vendula Poncová - xponco00
 * @projekt       IFJ11
 * @date
 */

#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "table.h"
#include "str.h"

#define EOK  0
#define ERR -5

// vestavene funkce:

int type  (TVarData *dest, TVarData *param);
int substr(TVarData *dest, TVarData *dataS, TVarData *dataFrom, TVarData *dataTo);
int find  (TVarData *dest, TVarData *sData, TVarData *sPatternData);
int sort  (TVarData *dest, TVarData *sData);

// funkce pro kmp:

int kmp        (char *text, char *vzor);
void kmpVektor (char *vzor, int vektor[]);
int kmpHledej  (char *text, char *vzor, int vektor[]);

// funkce pro merge sort :

void mergeSort  (char *str);
void copyArray  (char *dest, char *src, int strLen);
void copyToSrc  (int strLen, char *arr);
bool isSorted   (int strLen, char *arr);
void copyRight  (int *k, int *j, int *copied, char *arr, int step);
void copyLeft   (int *k, int *i, int *copied, char *arr, int step);


#endif // LIBRARY_H_INCLUDED
