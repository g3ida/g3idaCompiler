#ifndef LEX
#define LEX

#include <ctype.h>
#include <stdio.h>
#include "util.h"

#define MAX_LEN 60
#define NUM_RESERVED 15
#define ID_MAX_NUMBER 300

extern char lexeme[MAX_LEN];

#include <string.h>

Symbole analLex(void);
void printSymbole( Symbole token, const char* lexeme );

#endif // LEX
