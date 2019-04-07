#include "lexical.h"

FILE* input = NULL;
BOOL save = FALSE;
char lexeme[MAX_LEN];

static struct
    { char* str;
      Symbole tok;
    } reserved[NUM_RESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},{"var",VAR}, {"integer",INTEGER}, {"char",CHAR},
      {"do",DO},{"while",WHILE},{"read",READ}, {"readln",READLN}, {"writeln",WRITELN},
{"write",WRITE}, {"begin",BEGIN}, {"program",PROGRAM}};

void printSymbole( Symbole token, const char* lexeme )
{
    switch (token)
  { case IF: printf("keyword: if");break;
    case THEN: printf("keyword: then");break;
    case ELSE: printf("keyword: else");break;
    case END: printf("keyword: end");break;
    case DO: printf("keyword: do");break;
    case WHILE: printf("keyword: while");break;
    case PROGRAM: printf("keyword: program");break;
    case READ: printf("keyword: read");break;
    case READLN: printf("keyword: readln");break;
    case WRITE: printf("keyword: write");break;
    case WRITELN: printf("keyword: writeln");break;
    case INTEGER: printf("keyword: integer");break;
    case CHAR: printf("keyword: char");break;
    case VAR: printf("keyword: var");break;
    case BEGIN: printf("keyword: begin");break;
    case ASSIGN: printf(":="); break;
    case LT: printf("<"); break;
    case LTE: printf("<="); break;
    case GT: printf(">"); break;
    case GTE: printf(">="); break;
    case NEQ: printf("<>"); break;
    case AND: printf("&&"); break;
    case OR: printf("||"); break;
    case EQ: printf("=="); break;
    case LPAREN: printf("("); break;
    case RPAREN: printf(")"); break;
    case SEMI: printf(";"); break;
    case PLUS: printf("+"); break;
    case MINUS: printf("-"); break;
    case MULT: printf("*"); break;
    case DIV: printf("/"); break;
    case MOD: printf("%%"); break;
    case ENDFILE: printf("EOF"); break;
    case TWOPTS: printf(":"); break;
    case COMMA: printf(","); break;
    case POINT : printf("."); break;
    case NUM: printf("NUM, val= %s",lexeme);break;
    case ID: printf("ID, name= %s",lexeme);break;
    case ERROR: printf("ERROR: %s",lexeme);break;
    default: /* should never happen */
      printf("Unknown token: %d",token);
  }
}

static Symbole unilexId (char * s)
{ int i;
  for (i=0;i<NUM_RESERVED;i++)
    if (!strcmp(s,reserved[i].str))
      return reserved[i].tok;
  return ID;
}

typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID, INGEQ, INNEQ, INEQ, INAND, INOR, INPARENT, DONE }
StateType;

int lookback= 0;

Symbole analLex(void)
{
 /* index for storing into lexeme */
   int lexemeIndex = 0;
   /* holds current token to be returned */
   Symbole current;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to lexeme */
   int save;
   while (state != DONE)
   { int c = fgetc(input);
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
               save = FALSE; current = ENDFILE; break;
             case '%':
               current = MOD; break;
             case '+':
               current = PLUS; break;
             case '-':
               current = MINUS; break;
             case '*':
               current = MULT; break;
             case '/':
               current = DIV; break;
            case ',':
               current = COMMA; break;
            case '.':
               current = POINT; break;
             case ')':
               current = RPAREN; break;
             case ';':
               current = SEMI; break;
             default:
               current = ERROR; break;
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
            ungetc(c, input);
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
            int v = fgetc(input);
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
           ungetc(c, input);
           save = FALSE;
           current = TWOPTS;
           state = DONE;
         }
         break;
       case INNUM:
         if (!isdigit(c))
         { /* backup in the input */
           ungetc(c, input);
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
            ungetc(c, input);
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
          { ungetc(c, input);
            state = DONE;
            current = GT;
          }
          break;

         case INAND:
          if (c == '&') {
            state = DONE;
            current = AND;
          } else {
            ungetc(c, input);
            state = DONE;
           current = ERROR;
          }
          break;

        case INOR:
          if (c == '|') {
            state = DONE;
            current = OR;
          } else {
            ungetc(c, input);
            state = DONE;
           current = ERROR;
          }
         break;

        case INEQ:
          if (c == '=') {
            state = DONE;
            current = EQ;
          } else {
            ungetc(c, input);
            state = DONE;
           current = ERROR;
          }
         break;

       case INID:
         if (!isalpha(c) && !isdigit(c))
         { /* backup in the input */
           ungetc(c, input);
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
     if ((save) && (lexemeIndex <= MAX_LEN)) {
       lexeme[lexemeIndex++] = (char) c;
     }
     if (state == DONE)
     { lexeme[lexemeIndex] = '\0';
       if (current == ID)
         current = unilexId(lexeme);
     }
   }
   return current;
}
