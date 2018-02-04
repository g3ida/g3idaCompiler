#ifndef _UTIL_H_
#define _UTIL_H_
#include "lexical.h"
#include "globals.h"


extern int etiq;

#define NEXT_ETIQ etiq++

int addID(const char* name);
int idExists(const char* name);

extern int lineno;
extern IdField idTable[100];

const char* typeToStr(exprType type);

char * copyString(const char * );

#endif
