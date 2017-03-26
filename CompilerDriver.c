#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Compiler Driver for pl0 compiler
// Timothy Garrett
// James Williamson

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

    // Loop to check for -l, -a, and -v argument in command line
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp("-l",argv[i]) == 0){
            printLexFlag = 1;
        } else if(strcmp("-a",argv[i]) == 0){
            printCodeFlag = 1;
        } else if(strcmp("-v",argv[i]) == 0){
            printVMFlag = 1;
        } else{
            printf("\nArgument %s not recognized. The program will still run, but may not give the desired output\n\n", argv[i]);
        }
    }

    int errorCheck = 0;

    // Run the lexer. if it returns something other than a 0, quit the program.
	errorCheck = system("./Lexer.exe");
	if(errorCheck != 0){
		printf("\nGOT a lexer error.\n");
        	printLexFlag = 0;
        	errorCheck = 0;
		exit(1);
    }
    // If -l was input, print the lexer output
    if(printLexFlag)
        PrintLex();

    // Run the parser. If anything other than 0 is returned, quit the program.
	errorCheck = system("./Parser.exe");
	if(errorCheck != 0){
		printf("\nGOT a parser error.\n");
        	printCodeFlag = 0;
        	errorCheck = 0;
		exit(1);
    }
    // if -a was input, print out the assembly code
    if(printCodeFlag)
    	PrintCode();

    // Run the virtual machine. If anything other than 0 is returned, quit the program.
	errorCheck = system("./VirtualMachine.exe");
	if(errorCheck != 0){
		printf("\nGOT a VM error.\n");
        	printVMFlag = 0;
        	errorCheck = 0;
		exit(1);
    }
    // if -v was input, print out the stack from the virtual machine.
    if(printVMFlag)
        PrintVM();
	
    if(!printLexFlag && !printCodeFlag && !printVMFlag)
    	printf("\n\nin              out\n");
}

// functions to print from files, character by character.
void PrintLex(){
    FILE *f;

    // lexeme list
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

    // text version of lexeme list
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

    // assembly code
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

    // stack trace
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
