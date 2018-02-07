#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

BOOL Error = FALSE;
int errorCount = 0;

IdField idTable[100];
int idTableMax = 0;

int idExists(const char* name) {
    for(int i=0; i< idTableMax; ++i) {
        if(strcmp(idTable[i].id, name) == 0) {
            idTable[i].used++;
            return i;
        }
    }
    return -1;
}

int etiq = 0;


int addID(const char* name)
{
    int ret = idExists(name);
    if(ret == -1) {
        idTable[idTableMax].id = copyString(name);
        idTable[idTableMax].type = Void;
        idTable[idTableMax].used = 0;
        ret = ++idTableMax;
    }
    return ret;
}

//the code line
int lineno = 1;

const char* typeToStr(exprType type) {
    static const char* names[] ={
        "Void","Integer", "Char", "Boolean", "Error-Type"
    };
    return names[(int)type];
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(const char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    printf("Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}
