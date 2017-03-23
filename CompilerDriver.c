#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Still needs to be improved

void PrintLex();
void PrintCode();
void PrintVM();

int main(int argc, char **argv){
    // flag to print lex list
    int printLexFlag = 0;
    // flag to print assembly code
    int printCodeFlag = 0;
    // flag to print the virtual machine execution trace
    int printVMFlag = 0;


    int i;
    for(i = 0; i< argc; i++){
        if(!strcmp("-l",argv[i])){
            printLexFlag = 1;
        } else if(!strcmp("-a",argv[i])){
            printCodeFlag = 1;
        } else if(!strcmp("-v",argv[i])){
            printVMFlag = 1;
        }
    }

    int errorCheck = 0;

    if(printLexFlag){
        errorCheck = system("./Lexer.exe");
    }
    if(errorCheck < 0){
        //some kind of error for the lexer program not working

        printLexFlag = 0;
        errorCheck = 0;
    }

    if(printCodeFlag){
        errorCheck = system("./Parser.exe");
    }
    if(errorCheck < 0){
        //some kind of error for the code gen program not working

        printCodeFlag = 0;
        errorCheck = 0;
    }

    if(printVMFlag){
        errorCheck = system("./VirtualMachine.exe");
    }

    if(errorCheck < 0){
        //some kind of error for the virtual machine program not working

        printVMFlag = 0;
        errorCheck = 0;
    }

    printf("\n\n");

    if(printLexFlag){
        PrintLex();
    }

    printf("\n\n");

    if(printCodeFlag){
        PrintCode();
    }

    printf("\n\n");

    if(printVMFlag){
        PrintVM();
    }
}

void PrintLex(){
    FILE *f;

    f = fopen("lexList.txt", "r");

    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }

    fclose(f);

    printf("\n\n");

    f = fopen("tokenTypeText.txt", "r");

    c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }

    fclose(f);
}

void PrintCode(){
    FILE *f;

    f = fopen("CodeGenOutput.txt", "r");

    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }

    fclose(f);
}

void PrintVM(){
    FILE *f;

    f = fopen("VMOutput.txt", "r");

    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }

    fclose(f);
}
