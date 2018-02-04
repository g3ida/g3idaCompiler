#include "globals.h"
#include "util.h"
#include "lexical.h"
#include "parse.h"
#include <stdlib.h>

FILE* output = NULL;

//this flag is used for aller-si-faux et aller-si-vrai.
int CONDITION_FLAG = 0;


static Symbole token; /* holds current token */

/* function prototypes for recursive calls */
static void program(void);
static void dcl(void);
static void liste_id(void);
static exprType type(void);
static void inst_composee(void);
static void liste_inst(void);
static void statement(void);
static exprType expr(void);
static exprType simple_expr(void);
static exprType term(void);
static exprType factor(void);
static void if_stmt(void);
static void while_stmt(void);
static void assign_stmt(void);
static void read_stmt(void);
static void write_stmt(void);
static void readln_stmt(void);
static void writeln_stmt(void);

static void syntaxError(const char * message)
{ printf("\n>>> ");
  printf("ERROR : Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(Symbole exprected)
{ if (token == exprected) token = getSymbole();
  else {
    syntaxError("unexprected token -> ");
    printSymbole(token,tokenString);
    printf(" >> exprected token: ");
    printSymbole(exprected,"");

    printf("      ");
  }
}

void program(void)
{
    match(PROGRAM);
    addID(tokenString);
    match(ID);
    match(SEMI);
    dcl();
    inst_composee();
    match(POINT);
}

void dcl(void)
{
    while(token != BEGIN) {
        match(VAR);
        liste_id();
        match(TWOPTS);
        exprType t = type();
        for(int i=1; i< idTableMax; i++) {
            if(idTable[i].type == Void) {
                idTable[i].type = t;
            }
        }
        match(SEMI);
    }
}


exprType type(void)
{
    switch(token) {
    case INTEGER :
        match(INTEGER);
        return Integer;
        break;
    case CHAR :
        match(CHAR);
        return Char;
        break;
    default:
        return Void;
        break;
    }
}

void liste_id(void)
{
    if(addID(tokenString) == idTableMax-1) {
           printf("ERROR : variable '%s' declared twice\n", tokenString);
    }
    match(ID);
    while(token == COMMA) {
        match(COMMA);
        if(addID(tokenString) == idTableMax-1) {
           printf("ERROR : variable '%s' declared twice\n", tokenString);
        }
        match(ID);
    }
}

void inst_composee(void)
{
    match(BEGIN);
    if(token != END) {
        liste_inst();
    }
    match(END);
}

void liste_inst(void)
{
  statement();
  while ((token!=END))
  {
    match(SEMI);
    statement();
  }
}

void statement(void)
{
    switch (token) {
    case IF : if_stmt(); break;
    case WHILE :  while_stmt(); break;
    case ID : assign_stmt(); break;
    case READ : read_stmt(); break;
    case WRITE : write_stmt(); break;
    case READLN : readln_stmt(); break;
    case WRITELN : writeln_stmt(); break;
    default : syntaxError("unexprected token -> ");
              printSymbole(token,tokenString);
              token = getSymbole();
              break;
    } /* end case */
}

void if_stmt(void)
{
    int else_etiq = NEXT_ETIQ, end_etiq = NEXT_ETIQ;
    exprType t;
    match(IF);
    t = expr();
    if(t != Boolean) {
        printf("ERROR : at line %d: expected Boolean expression as condition for the if statement\n", lineno);
    }
    if(CONDITION_FLAG) {
        fprintf(output,"aller-si-vrai %d\n",else_etiq);
    } else {
        fprintf(output,"aller-si-faux %d\n",else_etiq);
    }
    match(THEN);
    statement();
    fprintf(output,"allera %d\n",end_etiq);
    match(ELSE);
    fprintf(output,"etiq %d\n",else_etiq);
    statement();
    fprintf(output,"etiq %d\n",end_etiq);
}

void while_stmt(void)
{
    int loop_etiq = NEXT_ETIQ, end_etiq = NEXT_ETIQ;
    match(WHILE);
    exprType t;
    fprintf(output,"etiq %d\n",loop_etiq);
    t = expr();
    if(t != Boolean) {
        printf("ERROR : at line %d: expected Boolean expression as condition for the while statement\n", lineno);
    }
    if(CONDITION_FLAG) {
        fprintf(output,"aller-si-vrai %d\n",end_etiq);
    } else {
        fprintf(output,"aller-si-faux %d\n",end_etiq);
    }
    match(DO);
    statement();
    fprintf(output,"allera %d\n",loop_etiq);
    fprintf(output,"etiq %d\n",end_etiq);
}

void assign_stmt(void)
{
    exprType trhs, tlhs;
    int tmp;
    if (token==ID)
        if((tmp = idExists(tokenString)) == -1) {
            printf("ERROR : variable '%s' undeclared\n", tokenString);
        }
    tlhs = idTable[tmp].type;
    fprintf(output,"valeurg %s\n",tokenString);
    match(ID);
    match(ASSIGN);
    trhs = simple_expr();
    if(tlhs != trhs) {
         if(trhs != TypeError)
            printf("ERROR : at line %d: type mismatch cannot convert %s to %s\n", lineno, typeToStr(trhs), typeToStr(tlhs));
    }
    fprintf(output,":=\n");
}

void read_stmt(void)
{
    match(READ);
    match(LPAREN);
    if (token==ID)
        if(idExists(tokenString) == -1) {
            printf("ERROR : variable '%s' undeclared\n", tokenString);
        }
    fprintf(output,"valeurg %s\n",tokenString);
    fprintf(output,"read\n");
    match(ID);
    match(RPAREN);
}

void readln_stmt(void)
{
    match(READLN);
    match(LPAREN);
    if (token==ID)
        if(idExists(tokenString) == -1) {
            printf("ERROR : variable '%s' undeclared\n", tokenString);
        }
    fprintf(output,"valeurg %s\n",tokenString);
    fprintf(output,"readln\n");
    match(ID);
    match(RPAREN);
}

void write_stmt(void)
{
    match(WRITE);
    match(LPAREN);
    if (token==ID)
        if(idExists(tokenString) == -1) {
            printf("ERROR : variable '%s' undeclared\n", tokenString);
        }
    fprintf(output,"valeurd %s\n",tokenString);
    fprintf(output,"write\n");
    match(ID);
    match(RPAREN);
}

void writeln_stmt(void)
{
    match(WRITELN);
    match(LPAREN);
    if (token==ID)
    if(idExists(tokenString) == -1) {
        printf("ERROR : variable '%s' undeclared\n", tokenString);
    }
    fprintf(output,"valeurd %s\n",tokenString);
    fprintf(output,"writeln\n");

    match(ID);
    match(RPAREN);
}

exprType expr(void)
{
    exprType tlhs, trhs;
    Symbole op;
    tlhs = simple_expr();
    op = token;
    if ((token==LT)||(token==EQ)||(token==NEQ)|| (token==LTE) || (token==GT) || (token==GTE)) {
        match(token);
        trhs = simple_expr();
        if(trhs != tlhs && tlhs !=TypeError) {
         printf("ERROR : at line %d: type mismatch cannot convert %s to %s\n", lineno, typeToStr(trhs), typeToStr(tlhs));
         tlhs = TypeError;
        }

        switch(op) {
        case LT:
            fprintf(output,"comparer-si-inf\n");
            CONDITION_FLAG = 0;
            break;
        case GT:
            fprintf(output,"comparer-si-sup\n");
            CONDITION_FLAG = 0;
            break;
        case EQ:
            fprintf(output, "comparer-si-egal\n");
            CONDITION_FLAG = 0;
            break;
        case LTE:
            fprintf(output,"comparer-si-sup\n");
            CONDITION_FLAG = 1;
            break;
        case GTE:
            fprintf(output,"comparer-si-inf\n");
            CONDITION_FLAG = 1;
            break;
        case NEQ:
            fprintf(output, "comparer-si-egal\n");
            CONDITION_FLAG = 1;
            break;
        default:
            break;
        }
        tlhs = Boolean;
    }
    return tlhs;
}

exprType simple_expr(void)
{
    exprType tlhs, trhs;
    tlhs = term();
    while ((token==PLUS)||(token==MINUS) || (token==OR))
    {
        fprintf(output,"%s\n", tokenString);
        if(token == OR) {
        if(tlhs != Boolean && tlhs !=TypeError) {
            printf("ERROR : at line %d: expected Boolean operand for 'OR' operator (found %s)\n",lineno, typeToStr(tlhs));
            tlhs = TypeError;
        }
      }
      match(token);
      trhs = term();
      if(trhs != tlhs && tlhs !=TypeError) {
         printf("ERROR : at line %d: type mismatch cannot convert %s to %s\n", lineno, typeToStr(trhs), typeToStr(tlhs));
         tlhs = TypeError;
      }
    }
    return tlhs;
}

exprType term(void)
{
    exprType tlhs, trhs;
    tlhs = factor();
    while ((token==MULT)||(token==DIV) || (token == MOD) || (token == AND))
    {
      fprintf(output,"%s\n", tokenString);
      if(token == AND) {
        if(tlhs != Boolean && tlhs !=TypeError) {
            printf("ERROR : at line %d: expected Boolean operand for 'AND' operator (found %s)\n",lineno, typeToStr(tlhs));
            tlhs = TypeError;
        }
      }
      match(token);
      trhs = factor();
      if(trhs != tlhs && tlhs !=TypeError) {
         printf("ERROR : at line %d: type mismatch cannot convert %s to %s\n", lineno, typeToStr(trhs), typeToStr(tlhs));
         tlhs = TypeError;
      }
    }
    return tlhs;
}

exprType factor(void)
{
    exprType t = Void;
    int p;
    switch (token) {
    case NUM :
        fprintf(output,"empiler %d\n", atoi(tokenString));
        match(NUM);
        t = Integer;
        break;
    case ID :
        fprintf(output,"valeurd %s\n",tokenString);
        if((p = idExists(tokenString)) == -1) {
            printf("ERROR : variable '%s' undeclared\n", tokenString);
        }
        t = idTable[p].type;
        match(ID);
      break;
    case LPAREN :
        match(LPAREN);
        t = simple_expr();
        match(RPAREN);
        break;
    default:
        fprintf(output,"halte\n");
        syntaxError("unexprected token -> ");
        printSymbole(token,tokenString);
        token = getSymbole();
        break;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
void parse(void)
{
  token = getSymbole();
  program();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
}
