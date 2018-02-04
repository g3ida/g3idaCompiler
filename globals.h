#pragma once

#include <stdio.h>
//#include "util.h"


extern FILE* input;
extern FILE* output;

typedef enum {FALSE, TRUE}BOOL;

typedef enum {
    ID, NUM,
    PROGRAM, VAR, BEGIN, END, INTEGER, CHAR, WHILE, DO, READ, WRITE, READLN, WRITELN, IF, THEN, ELSE,
    LPAREN, RPAREN, EQ, NEQ, LT, GT, LTE, GTE, PLUS, MINUS, OR, AND, MULT, DIV, MOD, ENDFILE, SEMI, ERROR, ASSIGN, TWOPTS, COMMA, POINT
}Symbole;


extern int idTableMax;

/* exprType is used for type checking */
typedef enum {Void,Integer, Char, Boolean, TypeError} exprType;

typedef struct {
    char* id;
    exprType type;
    int value;
    BOOL used;
}IdField;

extern BOOL Error;
