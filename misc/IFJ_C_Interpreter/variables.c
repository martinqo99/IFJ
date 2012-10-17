/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: variables.c - modul tabulky konstant a promenych
 * Autor: David Barina, xbarin02
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ifj05.h"
#include "variables.h"
#include "scanner.h"

/***
 * struct _TVariable
 *   vlastni typ promene (i konstanty)
 */
struct _TVariable
       {
         TVarType VarType;   // jaky je typ promene/konstanty
         void *VarValue;     // ukazatel na jeji hodnotu (nepracujte primo!)
                             //   (char **, int *, double *)
         PVariable next;     // ukazatel na dalsi prvek
       };


/***
 * ErrorFatal()
 *   moje pomocna funkce pro nahlaseni kriticke chyby a ukonceni programu
 *   vyuziva globalni funkci pro hlaseni chyb GlobalErr()
 *   parametry stejne jako funkce GlobalErr resp. printf()
 */
extern void ErrorFatal(const char *fmt, ...);

/***
 * ConstantsCreate()
 *   vytvori tabulku konstant a vrati jeji adresu 
 */
PConstantTable ConstantsCreate()
{
  PConstantTable table = (PConstantTable)malloc(sizeof(TConstantTable));
  if(table == NULL)
    ErrorFatal("Not enough of memory");
  table->first = NULL;
  return(table);
}

/***
 * ConstantsInsert()
 *   vlozi prvek do tabulky konstant a vrati ukazatel na nej
 *   1. parametr - typ konstanty
 *   2. parametr - adresa konstanty   
 */
PVariable ConstantsInsert(PConstantTable table, TVarType type, void *value)
{
#ifndef NDEBUG
  if(table == NULL)
    ErrorFatal("Internal error - inserting to uninitialized constant table");
#endif
  switch(type)
  {
    case vtUninit:
    {
#ifndef NDEBUG
      ErrorFatal("Internal error - constant cannot be uninitialized");
#endif
    } break;
    case vtInt:
    case vtFloat:
    case vtString:
    {
      PVariable *constant = &(table->first);
      while(*constant != NULL)
      {
        // konstanty neni treba vkladad vicekrat
        if((*constant)->VarType == type)
        {
          // typ se shoduje, shoduje se i obsah?
          switch(type)
          {
            case vtInt:
            {
              int *new_value = (int *)(value);
              int *cur_value = (int *)((*constant)->VarValue);
              if( (*new_value) == (*cur_value) )
              {
                free(value);
                return(*constant);
              }
            } break;
            case vtFloat:
            {
              double *new_value = (double *)(value);
              double *cur_value = (double *)((*constant)->VarValue);
              if( (*new_value) == (*cur_value) )
              {
                free(value);
                return(*constant);
              }
            } break;
            case vtString:
            {
              char *new_value = (char *)(value);
              char *cur_value = (char *)((*constant)->VarValue);
              if( 0 == strcmp((new_value),(cur_value)) )
              {
                free(value);
                return(*constant);
              }
            } break;
            default:
            {
              // sem se to nikdy nedostane
            }
          }
        }
        constant = &((*constant)->next);
      }
      // musime ji vlozit
      *constant = (PVariable)malloc(sizeof(TVariable));
      if( *constant == NULL)
        ErrorFatal("Not enough of memory");
      (*constant)->VarType = type;
      (*constant)->VarValue = value;
      (*constant)->next = NULL;
      return(*constant);
      
    } break;
    default:
    {
#ifndef NDEBUG
      ErrorFatal("Internal error - unknown variable type");
#endif
    }
  }
#ifndef NDEBUG
  ErrorFatal("Internal error - unhandled constant inserting");
#endif
  return(NULL);
}

/***
 * ConstantsDestroy()
 *   zrusi tabulku konstant i s obsahem (vsechny konstanty)
 */
void ConstantsDestroy(PConstantTable table)
{
#ifndef NDEBUG
  if(table == NULL)
    ErrorFatal("Internal error - destroying uninitialized table");
#endif
  PVariable tmp = table->first, tmp2;
  while(tmp != NULL)
  {
    tmp2 = tmp->next;
    free(tmp->VarValue);
    free(tmp);
    tmp = tmp2;
  }
  free(table);
}

/***
 * VariablesCreate()
 *   vytvori tabulku promenych a vrati jeji adresu 
 */
PVariableTable VariablesCreate()
{
  PVariableTable table = (PVariableTable)malloc(sizeof(TVariableTable));
  if(table == NULL)
    ErrorFatal("Not enough of memory");
  table->first = NULL;
  return(table);
}

/***
 * VariablesInsert()
 *   vlozi prvek do tabulky promenych a vrati ukazatel na nej
 *   promena je napocatku neinicializovana 
 *   parametr - tabulka promennych
 */
PVariable VariablesInsert(PVariableTable table)
{
#ifndef NDEBUG
  if(table == NULL)
    ErrorFatal("Internal error - inserting to uninitialized variable table");
#endif
  PVariable variable = (PVariable)malloc(sizeof(TVariable));
  variable->next = table->first;
  variable->VarType = vtUninit;
  variable->VarValue = NULL;
  table->first = variable;
  return(variable);
}

/***
 * VariablesDestroy()
 *   zrusi tabulku promenych i s jejim obsahem (vsechny promene)
 */
void VariablesDestroy(PVariableTable table)
{
#ifndef NDEBUG
  if(table == NULL)
    ErrorFatal("Internal error - destroying uninitialized table");
#endif
  PVariable tmp = table->first, tmp2;
  while(tmp != NULL)
  {
    tmp2 = tmp->next;
    free(tmp->VarValue);
    free(tmp);
    tmp = tmp2;
  }
  free(table);
}

/***
 * VariablesStore()
 *   funkce store (pro runtime), kt. ulozi do promene hodnotu
 *   1. parametr - promena
 *   2. parametr - typ noveho obsahu
 *   3. parametr - hodnota noveho obsahu (srara se uvolni)
 */
void VariablesStore(PVariable variable, TVarType type, void *value)
{
#ifndef NDEBUG
  if(variable == NULL)
    ErrorFatal("Internal error - NULL variable pointer");
  if(type == vtUninit)
    ErrorFatal("Internal error - could not uninitialize value");
#endif
  // free(NULL) je korektni operace, neprovede nic
  free(variable->VarValue);
  variable->VarType = type;
  variable->VarValue = value;
}

/***
 * VariablesGetType()
 *   funkce get_type (pro runtime) 
 *   vrati typ promene
 *   1. parametr - promena 
 */
TVarType VariablesGetType(PVariable variable)
{
#ifndef NDEBUG
  if(variable == NULL)
    ErrorFatal("Internal error - NULL variable pointer");
#endif
  return(variable->VarType);
}

/***
 * VariablesGetValue()
 *   funkce get_value (pro runtime)
 *   vrati obsah promene (nemente ji sami!) 
 *   1. parametr - promena 
 */
void *VariablesGetValue(PVariable variable)
{
#ifndef NDEBUG
  if(variable == NULL)
    ErrorFatal("Internal error - NULL variable pointer");
#endif
  return(variable->VarValue);
}

#ifndef NDEBUG
/***
 * ConstantPrint()
 *   vypise typ a obsah konstanty na stdout, ladici funkce
 *   parametr adresa konstanty  
 */
void ConstantPrint(PVariable constant)
{
  if(constant == NULL)
    ErrorFatal("Internal error - NULL constant pointer");
  switch(constant->VarType)
  {
    case vtInt:
    {
      printf("(int)    ");
      int *i = constant->VarValue;
      printf("%i",*i);
    } break;
    case vtFloat:
    {
      printf("(float)  ");
      double *d = constant->VarValue;
      printf("%g",*d);
    } break;
    case vtString:
    {
      printf("(string) ");
      char *c = constant->VarValue;
      printf("'%s'",c);
    } break;
    case vtUninit:
    {
      printf("(uninit) ERROR");
    } break;
    default:
    {
      printf("(unknown)ERROR");
    }
  }
}
#endif
