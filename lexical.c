#include "lexical.h"
#include "util.h"
#include "globals.h"

FILE* input = NULL;

BOOL save = FALSE;

char tokenString[MAX_LEN];

static struct
    { char* str;
      Symbole tok;
    } reserved[NUM_RESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},{"var",VAR}, {"integer",INTEGER}, {"char",CHAR},
      {"do",DO},{"while",WHILE},{"read",READ}, {"readln",READLN}, {"writeln",WRITELN},
{"write",WRITE}, {"begin",BEGIN}, {"program",PROGRAM}};

void printSymbole( Symbole token, const char* tokenString )
{
    switch (token)
  { case IF:
      printf("keyword: if\n");
      break;
    case THEN:
      printf("keyword: then\n");
      break;
    case ELSE:
      printf("keyword: else\n");
      break;
    case END:
      printf("keyword: end\n");
      break;
    case DO:
      printf("keyword: do\n");
      break;
    case WHILE:
      printf("keyword: while\n");
      break;
    case PROGRAM:
      printf("keyword: program\n");
      break;
    case READ:
      printf("keyword: read\n");
      break;
    case READLN:
      printf("keyword: readln\n");
      break;
    case WRITE:
      printf("keyword: write\n");
      break;
    case WRITELN:
      printf("keyword: writeln\n");
      break;
    case INTEGER:
      printf("keyword: integer\n");
      break;
    case CHAR:
      printf("keyword: char\n");
      break;
    case VAR:
    printf("keyword: var\n");
      break;
    case BEGIN:
      printf("keyword: begin\n");
      break;

    case ASSIGN: printf(":=\n"); break;
    case LT: printf("<\n"); break;
    case LTE: printf("<=\n"); break;
    case GT: printf(">\n"); break;
    case GTE: printf(">=\n"); break;
    case NEQ: printf("<>\n"); break;
    case AND: printf("&&\n"); break;
    case OR: printf("||\n"); break;
    case EQ: printf("==\n"); break;
    case LPAREN: printf("(\n"); break;
    case RPAREN: printf(")\n"); break;
    case SEMI: printf(";\n"); break;
    case PLUS: printf("+\n"); break;
    case MINUS: printf("-\n"); break;
    case MULT: printf("*\n"); break;
    case DIV: printf("/\n"); break;
    case MOD: printf("%%\n"); break;
    case ENDFILE: printf("EOF\n"); break;
    case TWOPTS: printf(":\n"); break;
    case COMMA: printf(",\n"); break;
    case POINT : printf(".\n"); break;
    case NUM:
      printf(
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      printf(
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      printf(
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      printf("Unknown token: %d\n",token);
  }
}


static Symbole reservedLookup (char * s)
{ int i;
  for (i=0;i<15;i++)
    if (!strcmp(s,reserved[i].str))
      return reserved[i].tok;
  return ID;
}

typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID, INGEQ, INNEQ, INEQ, INAND, INOR, INPARENT, DONE }
StateType;

int lookback= 0;

void ungetNextChar(int n)
{
    lookback = n;
}

int getNextChar(void)
{
    if(lookback != 0) {  int v = lookback; lookback = 0; return v;}
    return fgetc(input);
}

Symbole getSymbole(void)
{
 /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   Symbole current;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == ':')
           state = INASSIGN;
         else if ((c == ' ') || (c == '\t') || (c == '\n')) {
            if(c == '\n') lineno++;
           save = FALSE;
         }
         else if (c == '(')
         { save = FALSE;
           state = INPARENT;
         }
         else if (c == '=')
         { save = FALSE;
           state = INEQ;
         }
        else if (c == '<')
         { save = FALSE;
           state = INNEQ;
         }
        else if (c == '>')
         { save = FALSE;
           state = INGEQ;
         }
        else if (c == '&')
         { save = FALSE;
           state = INAND;
         }
        else if (c == '|')
         { save = FALSE;
           state = INOR;
         }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               current = ENDFILE;
               break;
             case '%':
               current = MOD;
               break;
             case '+':
               current = PLUS;
               break;
             case '-':
               current = MINUS;
               break;
             case '*':
               current = MULT;
               break;
             case '/':
               current = DIV;
               break;
            case ',':
               current = COMMA;
               break;
            case '.':
               current = POINT;
               break;
             case ')':
               current = RPAREN;
               break;
             case ';':
               current = SEMI;
               break;
             default:
               current = ERROR;
               break;
           }
         }
         break;
        case INPARENT:
          save = FALSE;
          if (c == '*'){
            state = INCOMMENT;
            break;
          } else {
            state = DONE;
            ungetNextChar(c);
            current = LPAREN;
          }
          break;

       case INCOMMENT:
         save = FALSE;
         if (c == EOF)
         { state = DONE;
           current = ENDFILE;
         } else if(c == '\n') {
            lineno++;
         }
         else if (c == '*') {
            int v = getNextChar();
            if(v == ')') {
                state = START;
                break;
            }
         }
         break;
       case INASSIGN:
         state = DONE;
         if (c == '=')
           current = ASSIGN;
         else
         { /* backup in the input */
           ungetNextChar(c);
           save = FALSE;
           current = TWOPTS;
           state = DONE;
         }
         break;
       case INNUM:
         if (!isdigit(c))
         { /* backup in the input */
           ungetNextChar(c);
           save = FALSE;
           state = DONE;
           current = NUM;
         }
         break;

         case INNEQ:
          if (c == '=') {
            state = DONE;
            current = LTE;
          } else if(c == '>') {
            state = DONE;
            current = NEQ;
          }
          else
          {
            ungetNextChar(c);
            state = DONE;
           current = LT;
          }
          break;

          case INGEQ:
          if (c == '=') {
            state = DONE;
            current = GTE;
          }
          else
          {
            ungetNextChar(c);
            state = DONE;
           current = GT;
          }
          break;

         case INAND:
          if (c == '&') {
            state = DONE;
            current = AND;
          } else {
            ungetNextChar(c);
            state = DONE;
           current = ERROR;
          }
          break;

        case INOR:
          if (c == '|') {
            state = DONE;
            current = OR;
          } else {
            ungetNextChar(c);
            state = DONE;
           current = ERROR;
          }
         break;

        case INEQ:
          if (c == '=') {
            state = DONE;
            current = EQ;
          } else {
            ungetNextChar(c);
            state = DONE;
           current = ERROR;
          }
         break;

       case INID:
         if (!isalpha(c) && !isdigit(c))
         { /* backup in the input */
           ungetNextChar(c);
           save = FALSE;
           state = DONE;
           current = ID;
         }
         break;
       case DONE:
       default: /* should never happen */
         printf("Scanner Bug\n");
         state = DONE;
         current = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAX_LEN)) {
       tokenString[tokenStringIndex++] = (char) c;
     }
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (current == ID)
         current = reservedLookup(tokenString);
     }
   }

    //printSymbole(current,tokenString);

   return current;
}
