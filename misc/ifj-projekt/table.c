/*
 * @description   Pomocna knihovna pro praci s tabulkou
 * @author        Marek Salat - xsalat00
 * @projekt       IFJ11
 * @date
 */

#include "table.h"

#define VAR_ALLOC_SIZE 8

TInstr *genInstr(EInstrType t, void *d, void *s1, void *s2) {
   TInstr *i = malloc(sizeof(TInstr));
   if(i == NULL)
      return NULL;
   i->type = t;
   i->dest = d;
   i->src1 = s1;
   i->src2 = s2;

   return i;
}

//----------------------------------------------------------------------

int varRealloc(TVar *v, int cnt) {
   if((cnt = v->alloc - cnt) >= 1)
      return INS_OK;

   cnt *=-1;
   cnt = cnt / VAR_ALLOC_SIZE + 1;

   int allocOld = v->alloc;
   v->alloc += cnt*VAR_ALLOC_SIZE;

   TVarData *tmp;
   if( ( tmp = realloc(v->varData, sizeof(TVarData)*v->alloc) ) == NULL)
      return INS_MALLOC;
   v->varData = tmp;

   for (int i = allocOld; i < v->alloc; i++) {
     v->varData[i].type = NIL;
   }

   return INS_OK;
}
//----------------------------------------------------------------------

void tableInit(TTable *T) {
   BTreeInit(&(T->functions), FUNCIONS);
   T->lastAddedFunc = NULL;
}

//----------------------------------------------------------------------

TVar *getLastAddedVar(TFunction *F) {
   return (TVar *)(F->variables.lastAdded->data);
}

//----------------------------------------------------------------------


int tableInsertFunction (TTable *T, string s) {

   TFunction *f  = malloc(sizeof(TFunction));
   char *newName = strCopyChar(&s);

   if(f == NULL || newName == NULL)
      return INS_MALLOC;

   BTreeInit(&(f->variables), VAR);
   listInit (&(f->constants));
   listInit (&(f->instructions));
   listInit (&(f->tmpVar));

   f->name = newName;
   f->cnt = -1;

   int err = BTreeInsert(&(T->functions), newName, f);
   if(err <= 0) { // neprobehl insert v poradku?
      free(f);
      free(newName);
      return err;
   }

   T->lastAddedFunc = f;
   // T->lastAddedFunc = ((TFunction *)T->functions.lastAdded->data);
   return INS_OK;
}

//----------------------------------------------------------------------

int functionInsertVar(TFunction *F, string s) {
   TVar     *v   = malloc(sizeof(TVar));
   if(v == NULL)
      return INS_MALLOC;

   TVarData *vd  = malloc(sizeof(TVarData)*VAR_ALLOC_SIZE);
   if(vd == NULL) {
      free(v);
      return INS_MALLOC;
   }

   for (int i = 0; i < VAR_ALLOC_SIZE; i++) {
     vd[i].type = NIL;
   }

   char *newName = strCopyChar(&s);
   if(newName == NULL) {
      free(v);
      free(vd);
      return INS_MALLOC;
   }

   v->name  = newName;
   v->alloc = VAR_ALLOC_SIZE;
   v->varData  = vd;
   v->varType  = VT_VAR;
   int err = BTreeInsert(&(F->variables), newName, v);
   if(err <= 0) {
      free(v);
      free(vd);
      free(newName);
      return err ;
   }
   return INS_OK;
}

//----------------------------------------------------------------------

int functionInsertConstatnt(TFunction *f, string attr, int token) {
   // vytvorim promenou
   TVar     *v   = malloc(sizeof(TVar));
   if(v == NULL)
      return INS_MALLOC;

   // vytvorim jeji data
   TVarData *vd  = malloc(sizeof(TVarData));
   if(vd == NULL) {
      free(v);
      return INS_MALLOC;
   }

   int err = INS_OK;
   // zjistim co mam ulozit a pak to tak ulozim
   switch(token) {
   case L_NUMBER: {
      vd->type = NUMBER;
      vd->value.n = atof ( attr.str );
   }
   break;
   case L_STRING: {
      vd->type = STRING;
      vd->value.s = strCreateString(&attr);
      if(strIsNull(&vd->value.s))
         err = INS_MALLOC;
   }
   break;
   case KW_TRUE: {
      vd->type = BOOL;
      vd->value.b = 1;
   }
   break;
   case KW_FALSE: {
      vd->type = BOOL;
      vd->value.b = 0;
   }
   break;
   case KW_NIL: {
      vd->type = NIL;
   }
   break;
   }

   v->name  = NULL;
   v->alloc = 1;
   v->varType = VT_CONST;
   v->varData = vd;

   if(listInsertLast(&f->constants, v) != LIST_EOK || err != INS_OK) {
      // kdyz se nepovedlo vlozit nebo pokud predtim u vytdareni dat
      // doslo k chybe musim vechno smazat
      free(v);
      if(vd->type == STRING)
         strFree(&(vd->value.s));
      free(vd);
      return INS_MALLOC;
   }

   return INS_OK;
}

//----------------------------------------------------------------------

TFunction *tableSearchFunction(TTable *T, string s) {
   TNode n = BTreeSearch(&(T->functions), s.str);
   return (n != NULL) ? (TFunction *)(n->data) : NULL;
}

//----------------------------------------------------------------------

TVar *functionSearchVar  (TFunction *F, string s) {
   TNode n = BTreeSearch(&(F->variables), s.str);
   return (n != NULL) ? (TVar *)(n->data) : NULL;
}

//----------------------------------------------------------------------

void freeConstTmpVarList(TList *l) {
   listFirst(l);
   while( l->Act != NULL ) {
      TVar *tmp = (TVar *)l->Act->data;

      if(tmp->varType == VT_VAR) {
         // jestli to byla promena tak musim projit cele pole a smazat vsechny stringy + jmeno
         for(int i = 0; i < tmp->alloc; i++) {
            if(tmp->varData[i].type == STRING)
               free(tmp->varData[i].value.s.str);
         }
         // jmeno jsem taky alokoval
         free(tmp->name);
      } else if(tmp->varType == VT_CONST || tmp->varType == VT_TMP_VAR) {
         // jestlize je to konstanta nebo pomocna promena tak overim jestli je string a pripadne smazu
         if(tmp->varData->type == STRING)
            free(tmp->varData->value.s.str);
      }
      // protoze sem musel pole alokovat tak ho musim smazat
      free(tmp->varData );
      free(tmp); // data jsem asi zrejmne taky alokoval tak je mazu

      listDeleteFirst(l);
      listFirst(l);
   }
}

void freeInstrList(TList *l) {
   listFirst(l);
   while( l->Act != NULL ) {
      free( l->Act->data );   // u instrukce byla data alokovana musim je smazat

      listDeleteFirst(l);
      listFirst(l);
   }
}

/*
 * pomocna fce pro tableClear, maze podle urceneho kontextu
 * bacha na to! maze strom funkci :)
 * @param   uzel strom
 * @param   typ stromu(urcuje predpis mazani dat)
 */
void clearNode(TNode n, EBTreeDataType type) {
   if(n != NULL) {
      clearNode(n->left,  type);
      clearNode(n->right, type);

      // tady uz muzu smazat polozku
      switch(type) {
         // predpis jak smazat data u funkce pokud jsou typu TFunction*
      case FUNCIONS: {
         TFunction *f = ((TFunction *)n->data);

         clearNode( f->variables.root, f->variables.type); // type by mel byt VAR
         freeConstTmpVarList( &(f->constants) );
         freeConstTmpVarList( &(f->tmpVar) );
         freeInstrList( &(f->instructions) );
         free(n->data);  // data jsem asi taky alokovala proto ji smazu
      }
      break;
      // predpis jak smazat data poku jsou typu TVar *
      case VAR: {
         TVar *temp = ((TVar *)n->data);
         for(int i = 0; i < temp->alloc; i++) {
            freeVarData(&temp->varData[i]);
         }
         free(temp->varData);
         free(n->data);
      }
      break;
      // nic nedelam
      case DEFAULT:
      default:
         break;
      }
      free(n->key);
      free(n);
   }
}


void  tableClear(TTable *T) {
   clearNode(T->functions.root, T->functions.type);
   tableInit(T);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// PRO DEBUG!!
FILE *out;

void listConstTmpPrint(TList *l) {
   listFirst(l);
   while(listActive(l)) {
      TVar *tmpV = ((TVar *)l->Act->data);
      TVarData *tmp = tmpV->varData;
      fprintf(out,"         ");
      if(tmpV->varType == VT_CONST) {
         switch( tmp->type ) {
         case STRING: {
            fprintf(out,"string \"%s\"", tmp[0].value.s.str);
         }
         break;
         case NUMBER: {
            fprintf(out,"number %g", tmp[0].value.n);
         }
         break;
         case BOOL: {
            fprintf(out,"bool %s", tmp[0].value.b == 1 ? "true" : "false");
         }
         break;
         case NIL: {
            fprintf(out,"nil");
         }
         break;
         default:
            fprintf(out,"unknow const");
         }
      } else
         fprintf(out,"$%d",  (int)tmpV);
      fprintf(out,"\n");
      listSucc(l);
   }
}

void printVar(TVar *src) {
   if(src != NULL) {
      if(src->varType == VT_VAR)
         fprintf(out," %s",src->name);
      else if( src->varType == VT_CONST) {
         switch(src->varData->type) {
         case STRING:
            fprintf(out," \"%s\"", src->varData->value.s.str);
            break;
         case NUMBER:
            fprintf(out," %g", src->varData->value.n);
            break;
         case BOOL:
            fprintf(out," %s", src->varData->value.b == 1 ? "true" : "false");
            break;
         case NIL:
            fprintf(out," nil");
            break;
         }
      } else {
         fprintf(out," $%d",  (int)src);
      }
   }
}
void listInstrPrint(TList *l) {
   listFirst(l);
   while(listActive(l)) {
      TInstr *tmp = ((TInstr *)l->Act->data);
      TVar *dst  = ((TVar *)tmp->dest);
      TVar *src1 = ((TVar *)tmp->src1);
      TVar *src2 = ((TVar *)tmp->src2);
      fprintf(out,"         ");

      switch (tmp->type) {
      case I_LAB: {
         fprintf(out,"LAB %d", (int)l->Act);
      }
      break;
      case I_RETURN:
         fprintf(out,"RETURN");
         break;
      case I_POP: {
         fprintf(out,"POP");
         printVar(dst);
      }
      break;
      case I_PUSH: {
         fprintf(out,"PUSH");
         printVar(dst);
      }
      break;
      case I_STACK_E: {
         fprintf(out,"STACK_E");
      }
      break;
      case I_MOV: {
         fprintf(out,"MOV");
         printVar(dst);
         printVar(src1);
      }
      break;
      case I_SET: {
         fprintf(out,"SET");
         printVar(dst);
         printVar(src1);
      }
      break;
      case I_ADD: {
         fprintf(out,"ADD");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_SUB: {
         fprintf(out,"SUB");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_MUL: {
         fprintf(out,"MUL");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_DIV: {
         fprintf(out,"DIV");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_POW: {
         fprintf(out,"POW");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CON: {
         fprintf(out,"CON");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_L: {
         fprintf(out,"CMP_L");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_LE: {
         fprintf(out,"CMP_LE");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_G: {
         fprintf(out,"CMP_G");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_GE: {
         fprintf(out,"CMP_GE");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_E: {
         fprintf(out,"CMP_E");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_CMP_NE: {
         fprintf(out,"CMP_NE");
         printVar(dst);
         printVar(src1);
         printVar(src2);
      }
      break;
      case I_JMP: {
         fprintf(out,"JMP %d",  (int)tmp->dest);
      }
      break;
      case I_JMP_Z: {
         fprintf(out,"JMP_Z");
         fprintf(out," %d",  (int)tmp->dest);
         printVar(src1);
      }
      break;
      case I_JMP_NZ: {
         fprintf(out,"JMP_NZ");
         fprintf(out," %d",  (int)tmp->dest);
         printVar(src1);
      }
      break;
      case I_WRITE: {
         fprintf(out,"WRITE");
         printVar(dst);
      }
      break;
      case I_READ: {
         fprintf(out,"READ");
         printVar(dst);
         printVar(src1);
      }
      break;

      case I_CALL: {
         fprintf(out,"CALL");
         fprintf(out," %s", ((TFunction *)tmp->dest)->name );
      }
      break;
      case I_TYPE: {
         fprintf(out,"TYPE");
         printVar(dst);
      }
      break;
      case I_SUBSTR: {
         fprintf(out,"SUBSTR");
         printVar(dst);
      }
      break;
      case I_FIND: {
         fprintf(out,"FIND");
         printVar(dst);
      }
      break;
      case I_SORT: {
         fprintf(out,"SORT");
         printVar(dst);
      }
      break;
      }

      fprintf(out,"\n");
      listSucc(l);
   }
}

//             uzel     typ stromu       oddelovac
void printNode(TNode n, EBTreeDataType t, char *delim) {
   if(n != NULL) {
      printNode(n->left, t, delim);

      switch(t) {
      case FUNCIONS: {
         fprintf(out,"\n %s%s\n    {\n", delim, n->key);

         TFunction *tmp   = n->data;
         fprintf(out, "     variables:\n");
         printNode( tmp->variables.root, tmp->variables.type /*VAR*/, "         " );
         fprintf(out,"\n     constants:\n");
         listConstTmpPrint(&tmp->constants);
         fprintf(out,"\n     tmp_var:\n");
         listConstTmpPrint(&tmp->tmpVar);
         fprintf(out,"\n     instruction:\n");
         listInstrPrint(&tmp->instructions);

         fprintf(out,"    }\n");
      }
      break;
      case VAR: {
         fprintf(out,"%s%s\n", delim, n->key);
      }
      break;
      case DEFAULT:
      default :
         break;
      }

      printNode(n->right, t, delim);
   }
}

void printTreeNodeOrder(TBTree *T) {
   printNode(T->root, T->type, "   ");
}

void tablePrintOrder(TTable table, FILE *f) {
   out = f;
   printTreeNodeOrder(&(table.functions)) ;
}
