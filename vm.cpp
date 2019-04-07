#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

enum InstructionSet {
    EMPILER,
    VALEURD,
    VALEURG,
    DEPILER,
    AFFECT,
    COPIER,
    MULT,
    DIV,
    PLUS,
    OR,
    AND,
    MINUS,
    MOD,
    ETIQ,
    ALLER_A,
    ALLER_SI_FAUX,
    ALLER_SI_VRAI,
    HALTE,
    COMPARER_SI_SUP,
    COMPARER_SI_INF,
    COMPARER_SI_EGAL,
    READ,
    WRITE,
    READLN,
    WRITELN,
    NUM_ELEMENTS
};

struct Inst {
    int value=0;
    std::string s;
    InstructionSet ins;
};

using namespace std;

std::vector<Inst> instr;
std::stack<Inst> s;

FILE* input = NULL;

std::map<std::string, int> memory;

std::map<int, int> etiq;

static struct
    { const char* str;
      InstructionSet is;
      int argc;
    } reserved[(int)NUM_ELEMENTS]
   = {{"empiler",EMPILER, 1},{"valeurd",VALEURD, 1},{"valeurg",VALEURG, 1},{"depiler",DEPILER, 0},{":=",AFFECT, 0},
        {"copier",COPIER, 0}, {"*",MULT, 0},{"/",DIV, 0},{"+",PLUS, 0},{"-",MINUS, 0}, {"||",OR, 0},
        {"&&",AND, 0}, {"etiq",ETIQ, 1}, {"%",MOD, 0}, {"aller-a",ALLER_A, 1}, {"aller-si-faux",ALLER_SI_FAUX, 1},
        {"aller-si-vrai", ALLER_SI_VRAI, 1}, {"halte",HALTE, 0}, {"comparer-si-sup",COMPARER_SI_SUP, 0},
        {"comparer-si-inf",COMPARER_SI_INF, 0}, {"comparer-si-egal",COMPARER_SI_EGAL, 0}, {"read",READ, 0},
        {"write",WRITE, 0}, {"readln",READLN, 0}, {"writeln",WRITELN, 0}};

static InstructionSet unilexId (const char * s)
{ int i;
  for (i=0;i<(int)NUM_ELEMENTS;i++)
    if (!strcmp(s,reserved[i].str))
      return reserved[i].is;
  return NUM_ELEMENTS;
}

static int unilexIdNumArgs (const char * s)
{ int i;
  for (i=0;i<(int)NUM_ELEMENTS;i++)
    if (!strcmp(s,reserved[i].str))
      return reserved[i].argc;
  return 0;
}

Inst getNextInst() {
    int c = fgetc(input);
    std::string s;
    while(c==' ' || c == '\n' || c =='\t') {
        c = fgetc(input);
    }

    while(c != EOF && c != ' ' && c !='\n')
    {
        s+= c;
        c = fgetc(input);
    }
    Inst i;
    i.ins = unilexId(s.c_str());

    if(unilexIdNumArgs(s.c_str()) == 0) return i;
    s = "";
    c = fgetc(input);
    while(c != EOF && c != '\n')
    {
        s+= c;
        c = fgetc(input);
    }

    if(i.ins == VALEURD || i.ins == VALEURG) {
        memory[s] = 0;
        i.s = s;
    } else{
        i.value = atoi(s.c_str());
    }
    return i;

}


int main(int argc, char* argv[])
{
    if(argc <= 1) {
        printf(" g3idaMachine: fatal error no input files\n");
        return 0;
    }

    input = fopen(argv[1], "r");
    if(input == NULL) {
        printf(" g3idaMachine: fatal error file not found %s\n", argv[1]);
        return -1;
    }

    while(true) {
        Inst is = getNextInst();
        if(is.ins == NUM_ELEMENTS) {break;}
        if(is.ins == ETIQ)
        {
            etiq[is.value] = instr.size();
        }
        instr.push_back(is);
    }

    unsigned int cc=0;
    while(cc !=instr.size()) {
        Inst i, j, k;
        //if(!s.empty())
        //cout << s.top().value << "++++"<<endl;
        //cout << s.size()<<endl;

        switch(instr[cc].ins) {
        case EMPILER: s.push(instr[cc]); break;
        case VALEURD: instr[cc].value = memory[instr[cc].s]; s.push(instr[cc]); break;
        case VALEURG: s.push(instr[cc]); break;
        case DEPILER: s.pop(); break;
        case AFFECT:
            i = s.top();s.pop();
            j = s.top();s.pop();
            memory[j.s] = i.value;
            break;
        case COPIER:
            i = s.top();
            s.push(i);
            break;
        case MULT:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value*i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case DIV:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value/i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case PLUS:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value+i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case OR:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value||i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case AND:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value&&i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case MINUS:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value-i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case MOD:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.value = j.value%i.value;
            k.ins = EMPILER;
            s.push(k);
            break;
        case ETIQ:
            break;
        case ALLER_A:
            cc = etiq[instr[cc].value];
            break;
        case ALLER_SI_FAUX:
            i = s.top();
            s.pop();
            if(i.value==0) {
                cc = etiq[instr[cc].value];
            }
            break;
        case ALLER_SI_VRAI:
            i = s.top();
            s.pop();
            if(i.value) {
                cc = etiq[instr[cc].value];
            }
            break;
        case HALTE:
            cc = instr.size();
            break;
        case COMPARER_SI_SUP:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.ins = EMPILER;
            k.value = 0;
            if(j.value > i.value) {
                k.value = 1;
            }
            s.push(k);
            break;
        case COMPARER_SI_INF:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.ins = EMPILER;
            k.value = 0;
            if(j.value < i.value) {
                k.value = 1;
            }
            s.push(k);
            break;
        case COMPARER_SI_EGAL:
            i = s.top();s.pop();
            j = s.top(); s.pop();
            k.ins = EMPILER;
            k.value = 0;
            if(i.value == j.value) {
                k.value = 1;
            }
            s.push(k);
            break;
        case READ:
            cin >> memory[s.top().s];
            s.pop();
            break;
        case WRITE:
            cout << s.top().value;
            s.pop();
            break;
        case READLN:
            cin >> memory[s.top().s];
            s.pop();
            cin.ignore();
            break;
        case WRITELN:
            cout << s.top().value<<endl;
            s.pop();
            break;
        default:
            break;
        }
        cc++;
    }

    //system("PAUSE");
    fclose(input);

    return 0;
}
