/*
 * Projekt: IFJ05
 * Resitele: xbarin02, xdudka00, xfilak01, xhefka00, xhradi08
 *
 * Soubor: scanner.c - modul lexikalniho analyzatoru
 * Autor: David Barina, xbarin02
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "ifj05.h"
#include "scanner.h"
#include "symbols.h"
#include "variables.h"

// - interni data -------------------------------------------------------------

/***
 * interni data scanneru, typ v .h souboru
 */ 
struct _TScannerData
       {
         FILE *f;
         int line;
         int c;
         PSymbols symbols;
         PConstantTable constants;
         PVariableTable variables;
       };

/***
 * TState
 *   stavy konecneho automatu scanneru
 */ 
typedef enum _TState
        {
          sInit,               // pocatecni stav
          sColon,              // nactena :
          sGreater,            // nactena >
          sLess,               // nactena <
          sExcl,               // nacten !
          sLiter,              // nacitani retezce
          sEsc,                // escape sekvence v retezci
          sId,                 // nacitani identifikatoru
          sInt,                // nacitany cislice
          sFloat,              // nacitany desetinne cislice
          sExp,                // nacteno e
          sExp2,               // nacitany cislice exponentu
          sSign,               // nacteno znamenko u exponentu
          sRem                 // nacitan komentar
        } TState;

/***
 * TCharBuffer
 *   pomocna struktura pro praci s nekonecnymi retezci
 */ 
typedef struct _TCharBuffer
        {
          int append_size;
          int allocated_size;
          int literal_size;
          char *ptr;
        } TCharBuffer;

// - funkci prototypy ---------------------------------------------------------

/***
 * ErrorFatal()
 *   - vypise chybu pres GlobalErr a ukonci program
 */ 
void ErrorFatal(const char *fmt, ...);

/***
 * PrintToken()
 *   - ladici funkce
 */ 
void PrintToken(TScannerToken *token);

/***
 * BufferCreate()
 *   - vytvoreni a inicializace nekonecneho retezce
 *   step_size - pocatecni velikost; kdyz pozdeji nestaci, prialokuje se
 *               prave tolik dalsich prvku
 */ 
TCharBuffer *BufferCreate(int step_size);

/***
 * BufferAppend()
 *   prida jeden znak do nekonecneho retezce
 */ 
int BufferAppend(TCharBuffer *buffer, char c);

/***
 * BufferDestroy()
 *   jestlize je buffer->ptr == NULL, retezec zustane v pameti;
 *   struktura se nici vzdy!
 */ 
int BufferDestroy(TCharBuffer *buffer);

/***
 * Str2Int()
 *   prevede retezec na int a vrati 0 - uspech, nenula - jinak
 */ 
int Str2Int(char *str, int *pInt);

/***
 * Str2Float()
 *   prevede retezec na double a vrati 0 - uspech, nenula - jinak
 */ 
int Str2Float(char *str, double *pDouble);

/***
 * NewChar()
 *   alokuje 1 char a vrati jeho adresu
 *   parametr znak
 */ 
char *NewChar(int i);

/***
 * ScannerInit()
 *   inicializace internich dat
 *   parametr - soubor
 */ 
TScannerData *ScannerInit(FILE *f);

/***
 * ScannerGetNextToken()
 *   nacte dalsi token, vyuziva ungetc(), token musi
 *   existovat (zalezitos volajici funkce)
 *   parametry - adresa dat a tokenu
 */
void ScannerGetNextToken(TScannerData *data, TScannerToken *token);

/***
 * ScannerDestroy()
 *   free internich dat
 *   parametr - adresa dat 
 */ 
void ScannerDestroy(TScannerData *data);

// - implementace -------------------------------------------------------------

void ErrorFatal(const char *fmt, ...)
{
  va_list pArgs;
  va_start(pArgs, fmt);
  // globalni funkce pro vypis chyb na stderr
  GlobalErr("Fatal error: ");
  GlobalErrV(fmt, pArgs);
  GlobalErr("\n");
  va_end(pArgs);
  exit(1);
}

TCharBuffer *BufferCreate(int step_size)
{
  TCharBuffer *tmp;
  tmp = (TCharBuffer *)malloc(sizeof(TCharBuffer));
  if(tmp == NULL)
    ErrorFatal("Not enought of memory");
  tmp->append_size = step_size;
  // napocatku nulova velikost, pak se prialokuje
  tmp->literal_size = 0;
  tmp->ptr = NULL;
  tmp->allocated_size = 0;
  return(tmp);
}

int BufferAppend(TCharBuffer *buffer, char c)
{
#ifndef NDEBUG
  if(buffer==NULL)
    ErrorFatal("Internal error - buffer unallocated");
#endif
  // vleze se tam jeste jeden dalsi znak?
  if(buffer->allocated_size > buffer->literal_size)
  {
    buffer->ptr[buffer->literal_size++] = c;
  }
  else
  {
    buffer->ptr = realloc(buffer->ptr, (buffer->allocated_size + 
                                        buffer->append_size) * sizeof(char) );
    if(buffer->ptr == NULL)
      ErrorFatal("Not enought of memory");
    buffer->allocated_size += buffer->append_size;
    // exponencialni alokace
    buffer->append_size <<= 1;
    buffer->ptr[buffer->literal_size++] = c;
  }
  return(0);
}

int BufferDestroy(TCharBuffer *buffer)
{
#ifndef NDEBUG
  if(buffer == NULL)
    ErrorFatal("Internal error - buffer unallocated");
#endif
  // free NULL je korektni, na to spoleham
  free(buffer->ptr);
  free(buffer);
  return(0);
}

int Str2Int(char *str, int *pInt)
{
  if(pInt == NULL)
    ErrorFatal("Not enought of memory");
  // tail a globalni errno - cist help
  char *tail;
  errno = 0;
  // v desitkove soustave (posledni parametr)
  *pInt = strtol(str, &tail, 10);
  if(errno)
    return(1);
  else
    return(0);
}

int Str2Float(char *str, double *pDouble)
{
  if(pDouble == NULL)
    ErrorFatal("Not enought of memory");
  char *tail;
  errno = 0;
  *pDouble = strtod(str, &tail);
  if(errno)
    return(1);
  else
    return(0);   
}

TScannerData *ScannerInit(FILE *f)
{
  TScannerData *data;
  data = (TScannerData *)malloc(sizeof(TScannerData));
  if(data == NULL)
    ErrorFatal("Not enought of memory");
  // soubor i s pozici
  data->f = f;
  data->line = 0;
  // netreba testovat na NULL
  data->symbols = SymbolsCreate();
  // netreba testovat na NULL
  data->constants = ConstantsCreate();
  // netreba testovat na NULL
  data->variables = VariablesCreate();
  return(data);
}

char *NewChar(int i)
{
  char *c = (char *)malloc(sizeof(char));
  if(c == NULL)
    ErrorFatal("Not enought of memory");
  *c = (char)i;
  return(c);
}

void ScannerDestroy(TScannerData *data)
{
  SymbolsDestroy(data->symbols);
  ConstantsDestroy(data->constants);
  VariablesDestroy(data->variables);
  free(data);
}

void ScannerGetNextToken(TScannerData *data, TScannerToken *token)
{
#ifndef NDEBUG
  if(data == NULL)
    ErrorFatal("Internal error - scanner data unallocated");
  if(token == NULL)
    ErrorFatal("Internal error - token unallocated");
#endif
  // zacina se vzdy ve stavu init
  TState state = sInit;
  int c;
  int done = 0;

  // pro cisla, retezce a identifikatory (cisla se pak ale prevedou)
  TCharBuffer *buffer;

  token->line = data->line;

  while(!done)
  {
    c = fgetc(data->f);
    // pro hlaseni chyb
    data->c = c;
    // pocitadlo, na kterem je token radku
    if(c == '\n')
      token->line = ++(data->line);
    // podle soucasneho stavu
    switch(state)
    {
      case sInit:
      {
        // a v kazdem stavu pro prichozi znak
        switch(c)
        {
          // jediny korektni konec souboru je ze stavu sInit
          case EOF :
          {
            token->tType = tEOF;
            done = 1;
          } break;
          case ' ' :
          case '\n':
          case '\t':
          case '\r':
          case '\f':
          {
            // preskocit oddelovac casti prikazu
          } break;
          case '#':
          {
            // zpracovani radkoveho komentare (rozsireni)
            state = sRem;
          } break;
          case '/':
          {
            token->tType = tOpDivide;
            done = 1;
          } break;
          case '*':
          {
            token->tType = tOpMultiply;
            done = 1;
          } break;
          case '-':
          {
            token->tType = tOpMinus;
            done = 1;
          } break;
          case '+':
          {
            token->tType = tOpPlus;
            done = 1;
          } break;
          case ',':
          {
            token->tType = tComma;
            done = 1;
          } break;
          case '(':
          {
            token->tType = tParenthesisLeft;
            done = 1;
          } break;
          case ')':
          {
            token->tType = tParenthesisRight;
            done = 1;
          } break;
          case ';':
          {
            token->tType = tSemicolon;
            done = 1;
          } break;
          case '=':
          {
            token->tType = tOpEqual;
            done = 1;
          } break;
          case ':':
          {
            state = sColon;
          } break;
          case '>':
          {
            state = sGreater;
          } break;
          case '<':
          {
            state = sLess;
          } break;
          case '!':
          {
            state = sExcl;
          } break;
          case '"':
          {
            // 32 znaku by mohlo na retezec stacit
            buffer = BufferCreate(32);
            state = sLiter;
          } break;
          default :
          {
            // case '0'..'9':
            if((c>='0')&&(c<='9'))
            {
              // 16 znaku by mohlo na int i double stacit
              buffer = BufferCreate(16);
              BufferAppend(buffer,c);
              state = sInt;
            }
            // case 'a'..'z':
            // case 'A'..'Z':
            else if(((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z')))
            {
              // 32 znaku by mohlo na identifikator stacit
              buffer = BufferCreate(32);
              BufferAppend(buffer,c);
              state = sId;
            }
            // jakykoli jiny znak (neni soucasti jazyka)
            else
            {
              token->tType = tInvalidChar;
              token->pData = &data->c;
              done = 1;
            }
          }
        }
      } break;
      case sColon:
      {
        switch(c)
        {
          case ':':
          {
            token->tType = tLabel;
            done = 1;
          } break;
          case '=':
          {
            token->tType = tAssignment;
            done = 1;
          } break;
          default :
          {
            token->tType = tInvalidColon;
            token->pData = &data->c;
            done = 1;
          }
        }
      } break;
      case sGreater:
      {
        switch(c)
        {
          case '=':
          {
            token->tType = tOpGreaterEqual;
            done = 1;
          } break;
          default :
          {
            token->tType = tOpGreater;
            done = 1;
            // nepochopil jsem, co ma za navratovou hodnotu
            // novy radek by rozhodil cislovani radku
            if(c!='\n')
              ungetc(c,data->f);
          }
        }
      } break;
      case sLess:
      {
        switch(c)
        {
          case '=':
          {
            token->tType = tOpLessEqual;
            done = 1;
          } break;
          default :
          {
            token->tType = tOpLess;
            done = 1;
            if(c!='\n')
              ungetc(c,data->f);
          }
        }
      } break;
      case sExcl:
      {
        switch(c)
        {
          case '=':
          {
            token->tType = tOpNotEqual;
            done = 1;
          } break;
          default :
          {
            token->tType = tInvalidExcl;
            token->pData = &data->c;
            done = 1;
          }
        }
      } break;
      case sLiter:
      {
        switch(c)
        {
          case '"':
          {
            // pridat znak '\0' na konec retezce
            BufferAppend(buffer,'\0');
            token->tType = tLitString;
            token->pData = ConstantsInsert(data->constants,vtString,
                                           buffer->ptr);
            //token->pData = buffer->ptr;
            done = 1;
            // nechci retezec uvolnit, zustave v tabulce konstant
            buffer->ptr = NULL;
            BufferDestroy(buffer);
          } break;
          case '\\':
          {
            // bude escape sekvence
            state = sEsc;
          } break;
          default :
          {
            // znaky s mensi ordinalni hodnotou nez 0x20 se nesmeji vyskytovat
            // v retezci
            if(c<0x20)
            {
              token->tType = tInvalidString;
              // ktery znak zpusobil chybu
              token->pData = &data->c;
              done = 1;
              BufferDestroy(buffer);
            }
            // ostatni znaky pridat do retezce
            else
            {
              BufferAppend(buffer,c);
            }
          }
        }
      } break;
      case sEsc:
      {
        switch(c)
        {
          case '\\':
          {
            BufferAppend(buffer,'\\');
            state = sLiter;
          } break;
          case 'n':
          {
            BufferAppend(buffer,'\n');
            state = sLiter;
          } break;
          case '"':
          {
            BufferAppend(buffer,'"');
            state = sLiter;
          } break;
          default :
          {
            token->tType = tInvalidEscape;
            token->pData = &data->c;
            done = 1;
            BufferDestroy(buffer);
          }
        }
      } break;
      case sId:
      {
        // skoda, ze C neumoznuje intervaly ve switch
        if(((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))||(c=='_'))
        {
          BufferAppend(buffer,c);
        }
        else
        {
          BufferAppend(buffer,'\0');
          // --- porovnani s klicovymi slovy ---
          if(0 == strcmp(buffer->ptr,"Heapsort"))
            token->tType = tFuncSort;
          else
          if(0 == strcmp(buffer->ptr,"go_to"))
            token->tType = tKeyGoTo;
          else
          if(0 == strcmp(buffer->ptr,"on"))
            token->tType = tKeyOn;
          else
          if(0 == strcmp(buffer->ptr,"print"))
            token->tType = tKeyPrint;
          else
          if(0 == strcmp(buffer->ptr,"read"))
            token->tType = tKeyRead;
          else
          {
            // jinak tu je urcite identifikator (case sensitive)
            token->tType = tIdentifier;
            token->pData = SymbolsInsert(data->symbols,buffer->ptr,
                                         data->variables);
            // nesmi se uvolni, je v tabulce symbolu
            buffer->ptr = NULL;
          }
          // ---
          done = 1;
          if(c!='\n')
            ungetc(c,data->f);
          BufferDestroy(buffer);
        }
      } break;
      case sInt:
      {
        switch(c)
        {
          case '.':
          {
            BufferAppend(buffer,c);
            state = sFloat;
          } break;
          default :
          {
            if((c>='0')&&(c<='9'))
            {
              BufferAppend(buffer,c);
            }
            else
            {
              BufferAppend(buffer,'\0');
              // --- prevod na int ---
              int *iInt = (int *)malloc(sizeof(int));
              if(Str2Int(buffer->ptr,iInt))
              {
                token->tType = tInvalidInt;
                token->pData = NULL;
                free(iInt);
              }
              else
              {
                token->tType = tLitInt;
                token->pData = ConstantsInsert(data->constants,vtInt,iInt);
                //token->pData = iInt;
              }
              // ---
              done = 1;
              if(c!='\n')
                ungetc(c,data->f);
              BufferDestroy(buffer);
            }
          }
        }
      } break;
      case sFloat:
      {
        switch(c)
        {
          // Float muze byt zadavan i s exponentem
          case 'e':
          {
            BufferAppend(buffer,c);
            state = sExp;
          } break;
          default :
          {
            if((c>='0')&&(c<='9'))
            {
              BufferAppend(buffer,c);
            }
            else
            {
              BufferAppend(buffer,'\0');
              // --- prevod na double ---
              double *dDouble = (double *)malloc(sizeof(double));
              if(Str2Float(buffer->ptr,dDouble))
              {
                token->tType = tInvalidFloat;
                token->pData = NULL;
                free(dDouble);
              }
              else
              {
                token->tType = tLitFloat;
                token->pData = ConstantsInsert(data->constants,vtFloat,dDouble);
                //token->pData = dDouble;
              }
              // ---
              done = 1;
              if(c!='\n')
                ungetc(c,data->f);
              BufferDestroy(buffer);
            }
          }
        }
      } break;
      case sExp:
      {
        switch(c)
        {
          // exponent muze obsahovaz znamenko
          case '+':
          case '-':
          {
            BufferAppend(buffer,c);
            state = sSign;
          } break;
          default :
          {
            if((c>='0')&&(c<='9'))
            {
              BufferAppend(buffer,c);
              state = sExp2;
            }
            else
            {
              token->tType = tInvalidExp;
              token->pData = &data->c;
              done = 1;
              BufferDestroy(buffer);
            }
          }
        }
      } break;
      case sExp2:
      {
        // ciste cislo exponentu
        if((c>='0')&&(c<='9'))
        {
          BufferAppend(buffer,c);
        }
        else
        {
          BufferAppend(buffer,'\0');
          // --- prevod na double ---
          double *dDouble = (double *)malloc(sizeof(double));
          if(Str2Float(buffer->ptr,dDouble))
          {
            token->tType = tInvalidFloat;
            token->pData = NULL;
            free(dDouble);
          }
          else
          {
            token->tType = tLitFloat;
            token->pData = ConstantsInsert(data->constants,vtFloat,dDouble);
            //token->pData = dDouble;
          }
          // ---
          done = 1;
          if(c!='\n')
            ungetc(c,data->f);
          BufferDestroy(buffer);
        }
      } break;
      case sSign:
      {
        // po znamenku smi byt jen ciste cislo exponentu
        if((c>='0')&&(c<='9'))
        {
          BufferAppend(buffer,c);
          state = sExp2;
        }
        else
        {
          token->tType = tInvalidSign;
          token->pData = &data->c;
          done = 1;
          BufferDestroy(buffer);
        }
      } break;
      case sRem:
      {
        if(c == '\n')
        {
          state = sInit;
        }
      } break;
      default:
      {
        // sem by se to nikdy nemelo dostat
#ifndef NDEBUG
        ErrorFatal("Internal error - unknown state");
#endif
      }
    }
  }
}
