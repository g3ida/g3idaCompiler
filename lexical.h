#ifndef LEX
#define LEX


#include <ctype.h>
#include <stdio.h>
#include "globals.h"

#define MAX_LEN 60
#define NUM_RESERVED 15
#define ID_MAX_NUMBER 300

extern char tokenString[MAX_LEN];


#include <string.h>

int getNextChar();
void ungetNextChar(int c);
Symbole getSymbole(void);
void printSymbole( Symbole token, const char* tokenString );



#endif // LEX
