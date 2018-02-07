#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h> //for FILE

/* exprType is used for type checking */
typedef enum {Void,Integer, Char, Boolean, TypeError} exprType;
typedef enum {FALSE, TRUE} BOOL;
typedef struct {
    char* id;
    exprType type;
    int value;
    BOOL used;
}IdField;

typedef enum {
    ID, NUM,
    PROGRAM, VAR, BEGIN, END, INTEGER, CHAR, WHILE, DO, READ, WRITE, READLN, WRITELN, IF, THEN, ELSE,
    LPAREN, RPAREN, EQ, NEQ, LT, GT, LTE, GTE, PLUS, MINUS, OR, AND, MULT, DIV, MOD, ENDFILE, SEMI, ERROR, ASSIGN, TWOPTS, COMMA, POINT
}Symbole;

extern BOOL Error;
extern int errorCount;
extern FILE* input;
extern FILE* output;
extern int lineno;
extern IdField idTable[100];
extern int idTableMax;
extern int etiq;
#define NEXT_ETIQ etiq++

int addID(const char* name);
int idExists(const char* name);
const char* typeToStr(exprType type);
char * copyString(const char * );

#endif
