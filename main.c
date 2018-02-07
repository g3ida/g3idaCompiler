#include <stdio.h>
#include <stdlib.h>

#include "lexical.h"
#include "util.h"
#include "parse.h"

int main(int argc, char* argv[])
{
    if(argc <= 1) {
        printf(" g3idaCompiler: fatal error no input files\n");
        return 0;
    }

    input = fopen(argv[1], "r");
    if(input == NULL) {
        printf(" g3idaCompiler: fatal error file not found %s\n", argv[1]);
        return -1;
    }

    output = fopen("out.txt", "w");

    parse();

    for(int i=1; i<idTableMax; ++i) {
        if(!idTable[i].used)
        printf("WARNING : variable '%s' declared but not used\n", idTable[i].id);
    }
/*
    for(int i=0; i<idTableMax; ++i) {
        printf(" '%s' ---- %s\n", idTable[i].id, typeToStr(idTable[i].type));
    }
*/
    fclose(input);
    fclose(output);


    #ifdef __WIN32
        system("PAUSE");
    #endif

    return 0;
}
