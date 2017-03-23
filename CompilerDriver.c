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
    for(i = 0; i < argc; i++){
        if(strcmp("-l",argv[i]) == 0){
            printLexFlag = 1;
        } else if(strcmp("-a",argv[i]) == 0){
            printCodeFlag = 1;
        } else if(strcmp("-v",argv[i]) == 0){
            printVMFlag = 1;
        }
    }

    int errorCheck = 0;

	errorCheck = system("./Lexer1.exe");
	if(errorCheck != 0){
        	//some kind of error for the lexer program not working
		printf("\nGOT a lexer error.\n");
        	printLexFlag = 0;
        	errorCheck = 0;
		exit(1);
    	}
    if(printLexFlag){
	PrintLex();
    }
	errorCheck = system("./Parser1.exe");
	if(errorCheck != 0){
        	//some kind of error for the code gen program not working
		printf("\nGOT a parser error.\n");
        	printCodeFlag = 0;
        	errorCheck = 0;
		exit(1);
    	}
    if(printCodeFlag){    
	// It must be ok so we print out the Lexers Output.
    	PrintCode();
    }
	errorCheck = system("./VirtualMachine1.exe");
	if(errorCheck != 0){
        	//some kind of error for the virtual machine program not working
		printf("\nGOT a VM error.\n");
        	printVMFlag = 0;
        	errorCheck = 0;
		exit(1);
    	}
    if(printVMFlag){
	PrintVM();
    }
}

void PrintLex(){
    FILE *f;

    f = fopen("lexList.txt", "r");
    if(f == NULL){
    	printf("We could not open the lexList.txt file! Exiting the program.\n");
	exit(1);
    }
    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }
    printf("\n\n");

    fclose(f);
    
    FILE *fp = fopen("tokenTypeText.txt", "r");

    if(fp == NULL){
    	printf("We could not open the tokenTypeText.txt file! Exiting the program.\n");
	exit(1);
    }

    c = getc(fp);

    while(c != EOF){
	printf("%c", c);
	c = getc(fp);
    }
    printf("\n\n");

    fclose(fp);
}

void PrintCode(){
    FILE *f;

    f = fopen("CodeGenOutput.txt", "r");

    if(f == NULL){
    	printf("We could not open the CodeGenOutput.txt file! Exiting the program.\n");
	exit(1);
    }

    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }
    printf("\n\n");

    fclose(f);
}

void PrintVM(){
    FILE *f;

    f = fopen("VMOutput.txt", "r");
    if(f == NULL){
	printf("We could not open the VMOutput.txt file! Exiting the program.\n");
	exit(1);
    }

    char c = getc(f);

    while(c != EOF){
        printf("%c", c);
        c = getc(f);
    }
    printf("\n\n");
    fclose(f);
}
