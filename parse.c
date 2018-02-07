#include "util.h"
#include "lexical.h"
#include "parse.h"
#include <stdlib.h>

FILE* output = NULL;

#define EMETTRE(...) fprintf(output, __VA_ARGS__)

#define ERROR_MSG(...)  printf("ERROR : error at line %d: ",lineno); \
                        printf(__VA_ARGS__); errorCount++; Error = TRUE

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

static void accepter(Symbole expected)
{ if (token == expected) token = analLex();
  else {
    ERROR_MSG("unexpected symbol '");
    printSymbole(token,lexeme);
    printf("'. Expected : '");
    printSymbole(expected,"");
    printf("\n");
  }
}

void program(void)
{
    accepter(PROGRAM);
    addID(lexeme);
    accepter(ID);
    accepter(SEMI);
    dcl();
    inst_composee();
    accepter(POINT);
}

void dcl(void)
{
    while(token != BEGIN) {
        accepter(VAR);
        liste_id();
        accepter(TWOPTS);
        exprType t = type();
        for(int i=1; i< idTableMax; i++) {
            if(idTable[i].type == Void) {
                idTable[i].type = t;
            }
        }
        accepter(SEMI);
    }
}


exprType type(void)
{
    switch(token) {
    case INTEGER :
        accepter(INTEGER);
        return Integer;
        break;
    case CHAR :
        accepter(CHAR);
        return Char;
        break;
    default:
        return Void;
        break;
    }
}

void liste_id(void)
{
    if(addID(lexeme) == idTableMax-1) {
           ERROR_MSG("variable '%s' declared twice\n", lexeme);
    }
    accepter(ID);
    while(token == COMMA) {
        accepter(COMMA);
        if(addID(lexeme) == idTableMax-1) {
           ERROR_MSG("variable '%s' declared twice\n", lexeme);
        }
        accepter(ID);
    }
}

void inst_composee(void)
{
    accepter(BEGIN);
    if(token != END) {
        liste_inst();
    }
    accepter(END);
}

void liste_inst(void)
{
  statement();
  while ((token!=END))
  {
    accepter(SEMI);
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
    default : ERROR_MSG("unexprected token ");
              printSymbole(token,lexeme);
              printf("\n");
              token = analLex();
              break;
    } /* end case */
}

void if_stmt(void)
{
    int else_etiq = NEXT_ETIQ, end_etiq = NEXT_ETIQ;
    exprType t;
    accepter(IF);
    t = expr();
    if(t != Boolean) {
        ERROR_MSG("expected Boolean expression as condition for the if statement\n");
    }
    if(CONDITION_FLAG) {
        EMETTRE("aller-si-vrai %d\n",else_etiq);
    } else {
        EMETTRE("aller-si-faux %d\n",else_etiq);
    }
    accepter(THEN);
    statement();
    EMETTRE("aller-a %d\n",end_etiq);
    accepter(ELSE);
    EMETTRE("etiq %d\n",else_etiq);
    statement();
    EMETTRE("etiq %d\n",end_etiq);
}

void while_stmt(void)
{
    int loop_etiq = NEXT_ETIQ, end_etiq = NEXT_ETIQ;
    accepter(WHILE);
    exprType t;
    EMETTRE("etiq %d\n",loop_etiq);
    t = expr();
    if(t != Boolean) {
        ERROR_MSG("expected Boolean expression as condition for the while statement\n");
    }
    if(CONDITION_FLAG) {
        EMETTRE("aller-si-vrai %d\n",end_etiq);
    } else {
        EMETTRE("aller-si-faux %d\n",end_etiq);
    }
    accepter(DO);
    statement();
    EMETTRE("aller-a %d\n",loop_etiq);
    EMETTRE("etiq %d\n",end_etiq);
}

void assign_stmt(void)
{
    exprType trhs, tlhs;
    int tmp;
    if (token==ID)
        if((tmp = idExists(lexeme)) == -1) {
            ERROR_MSG("variable '%s' undeclared\n", lexeme);
        }
    tlhs = idTable[tmp].type;
    EMETTRE("valeurg %s\n",lexeme);
    accepter(ID);
    accepter(ASSIGN);
    trhs = simple_expr();
    if(tlhs != trhs) {
         if(trhs != TypeError)
            ERROR_MSG("type mismatch cannot convert %s to %s\n", typeToStr(trhs), typeToStr(tlhs));
    }
    EMETTRE(":=\n");
}

void read_stmt(void)
{
    accepter(READ);
    accepter(LPAREN);
    if (token==ID)
        if(idExists(lexeme) == -1) {
            ERROR_MSG("variable '%s' undeclared\n", lexeme);
        }
    EMETTRE("valeurg %s\n",lexeme);
    EMETTRE("read\n");
    accepter(ID);
    accepter(RPAREN);
}

void readln_stmt(void)
{
    accepter(READLN);
    accepter(LPAREN);
    if (token==ID)
        if(idExists(lexeme) == -1) {
            ERROR_MSG("variable '%s' undeclared\n", lexeme);

        }
    EMETTRE("valeurg %s\n",lexeme);
    EMETTRE("readln\n");
    accepter(ID);
    accepter(RPAREN);
}

void write_stmt(void)
{
    accepter(WRITE);
    accepter(LPAREN);
    if (token==ID)
        if(idExists(lexeme) == -1) {
            ERROR_MSG("variable '%s' undeclared\n", lexeme);

        }
    EMETTRE("valeurd %s\n",lexeme);
    EMETTRE("write\n");
    accepter(ID);
    accepter(RPAREN);
}

void writeln_stmt(void)
{
    accepter(WRITELN);
    accepter(LPAREN);
    if (token==ID)
    if(idExists(lexeme) == -1) {
        ERROR_MSG("variable '%s' undeclared\n", lexeme);
    }
    EMETTRE("valeurd %s\n",lexeme);
    EMETTRE("writeln\n");

    accepter(ID);
    accepter(RPAREN);
}

exprType expr(void)
{
    exprType tlhs, trhs;
    Symbole op;
    tlhs = simple_expr();
    op = token;
    if ((token==LT)||(token==EQ)||(token==NEQ)|| (token==LTE) || (token==GT) || (token==GTE)) {
        accepter(token);
        trhs = simple_expr();
        if(trhs != tlhs && tlhs !=TypeError) {
            ERROR_MSG("type misaccepter cannot convert %s to %s\n", typeToStr(trhs), typeToStr(tlhs));

         tlhs = TypeError;
        }

        switch(op) {
        case LT:
            EMETTRE("comparer-si-inf\n");
            CONDITION_FLAG = 0;
            break;
        case GT:
            EMETTRE("comparer-si-sup\n");
            CONDITION_FLAG = 0;
            break;
        case EQ:
            EMETTRE( "comparer-si-egal\n");
            CONDITION_FLAG = 0;
            break;
        case LTE:
            EMETTRE("comparer-si-sup\n");
            CONDITION_FLAG = 1;
            break;
        case GTE:
            EMETTRE("comparer-si-inf\n");
            CONDITION_FLAG = 1;
            break;
        case NEQ:
            EMETTRE( "comparer-si-egal\n");
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
    char* opstr;
    tlhs = term();
    while ((token==PLUS)||(token==MINUS) || (token==OR))
    {
        opstr = copyString(lexeme);
        if(token == OR) {
        if(tlhs != Boolean && tlhs !=TypeError) {
            ERROR_MSG("expected Boolean operand for 'OR' operator (found %s)\n", typeToStr(tlhs));
            tlhs = TypeError;
        }
      }
      accepter(token);
      trhs = term();
      if(trhs != tlhs && tlhs !=TypeError) {
         ERROR_MSG("type mismatch cannot convert %s to %s\n", typeToStr(trhs), typeToStr(tlhs));
         tlhs = TypeError;
      }
      EMETTRE("%s\n", opstr);
      free(opstr);
    }
    return tlhs;
}

exprType term(void)
{
    exprType tlhs, trhs;
    char* opstr;
    tlhs = factor();
    while ((token==MULT)||(token==DIV) || (token == MOD) || (token == AND))
    {
      opstr = copyString(lexeme);
      if(token == AND) {
        if(tlhs != Boolean && tlhs !=TypeError) {
            ERROR_MSG("expected Boolean operand for 'AND' operator (found %s)\n", typeToStr(tlhs));
            tlhs = TypeError;
        }
      }
      accepter(token);
      trhs = factor();
      if(trhs != tlhs && tlhs !=TypeError) {
         ERROR_MSG("type mismatch cannot convert %s to %s\n", typeToStr(trhs), typeToStr(tlhs));
         tlhs = TypeError;
      }
      EMETTRE("%s\n", opstr);
      free(opstr);
    }
    return tlhs;
}

exprType factor(void)
{
    exprType t = Void;
    int p;
    switch (token) {
    case NUM :
        EMETTRE("empiler %d\n", atoi(lexeme));
        accepter(NUM);
        t = Integer;
        break;
    case ID :
        EMETTRE("valeurd %s\n",lexeme);
        if((p = idExists(lexeme)) == -1) {
            ERROR_MSG("variable '%s' undeclared\n", lexeme);
        }
        t = idTable[p].type;
        accepter(ID);
      break;
    case LPAREN :
        accepter(LPAREN);
        t = simple_expr();
        accepter(RPAREN);
        break;
    default:
        EMETTRE("halte\n");
        ERROR_MSG("unexprected symbol '");
        printSymbole(token,lexeme);
        printf("' \n");
        printSymbole(token,lexeme);
        token = analLex();
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
  token = analLex();
  program();
  printf("compilation ended total with %d errors\n", errorCount);
  if(Error) {
    #ifdef __linux__
        system("rm out.txt");
    #else
    #ifdef __WIN32
        system("del /f out.txt");
    #endif
    #endif
  }
}
