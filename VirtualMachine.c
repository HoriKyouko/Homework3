#include <stdio.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define MAX_REGISTERS 16

// Virtual Machine for Pl0 Compiler
// Timothy
// James Williamson

typedef struct {
int op;		// op code
int r;      // register
int  l;   	// level
int  m;   	// moffset
} instruction;

void traceCode(FILE * f,int line, instruction current);
void execute(instruction current, int * pc, int * sp, int * bp, int * registers, int * stack, int * hlt, int * currentLevel, int * ARs);
void printInfo(FILE * f,int pc, int sp, int bp, int* stack, int * ARs);
int base(int * stack, int l, int base);

int main(int argc, char * argv[]){
    // array of all lines of code
    instruction code[MAX_CODE_LENGTH];
    // the current line of code
    instruction ir = {.op = 0, .l = 0, .m = 0};
    // srray of registers
    int registers[MAX_REGISTERS] = {0};

    int ARs[MAX_LEXI_LEVELS] = {0};
    ARs[0] = 1;

	int sp = 0;
	int bp = 1;
	int pc = 0;
	int currentLevel = 0;

	int hlt = 0;

	// total size of input in lines
	int totalSize = 0;

    //
	int numInst = 0;

	int stack[MAX_STACK_HEIGHT] = {0};

	FILE *pl0Code;

    pl0Code = fopen("CodeGenOutput.txt", "r");

    if(pl0Code!=NULL){
        // scanning in data
        while(!feof(pl0Code) && totalSize<MAX_CODE_LENGTH){
            fscanf(pl0Code, "%d %d %d %d", &code[totalSize].op, &code[totalSize].r, &code[totalSize].l, &code[totalSize].m);
            totalSize++;
        }
        totalSize--;

        fclose(pl0Code);

        FILE *output;
        output = fopen("VMOutput.txt", "w");

        fprintf(output, "Initial Values:\t\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "r", "l", "m", "pc", "bp", "sp", "stack");

        // Runs the program
        while(!hlt && pc <= totalSize && bp > 0){
            // fetch
            ir = code[pc];
            traceCode(output, pc, ir);
            pc++;

            // execute
            execute(ir, &pc, &sp, &bp, registers, stack, &hlt, &currentLevel, ARs);
            printInfo(output, pc,sp,bp, stack, ARs);
        }

        fprintf(output, "\nRegisters:\n");

        int i;

        for(i = 0; i<MAX_REGISTERS; i++){
            fprintf(output, "R%d: %d ", i, registers[i]);
            if((i+1) % 8 == 0){
                fprintf(output, "\n");
            }
        }
    } else{
        printf("CodeGenOutput.txt not found\n");
    }

   return 0;
}

// Prints the stack
void printInfo(FILE * f, int pc, int sp, int bp, int* stack, int*ARs){
    int j = 1;
    fprintf(f, "\t%d\t%d\t%d   ", pc, bp, sp);
    for(int i = 1; i<=sp; i++){
        // AR separater
        if(i>1 && i == ARs[j] && bp < sp){
            fprintf(f, " |");
            j++;
        }
        fprintf(f, " %d", stack[i]);
    }
    fprintf(f, "\n");

}

void execute(instruction current, int * pc, int * sp, int * bp, int * registers, int * stack, int * hlt, int * currentLevel, int* ARs){

    switch(current.op){
        // LIT
		case 1:
			registers[current.r] = current.m;
			break;
        // RTN
        case 2:
            *sp = *bp - 1;
            *bp = stack[*sp + 3];
            if(*currentLevel > 0){
                ARs[*currentLevel] = 0;
                *currentLevel = *currentLevel -1;
            }
            *pc = stack[*sp + 4];
            break;
        // LOD
        case 3:
            registers[current.r] = stack[base(stack,current.l,*bp) + current.m];
            break;
        // STO
        case 4:
            stack[base(stack,current.l,*bp) + current.m] = registers[current.r];
            break;
        // CAL
        case 5:
            stack[*sp+1] = 0;
            stack[*sp+2] = base(stack,current.l, *bp);
            stack[*sp+3] = *bp;
            stack[*sp+4] = *pc;
            *bp = *sp+1;
            *currentLevel = *currentLevel+1;
            ARs[*currentLevel] = *bp;
            *pc = current.m;
            break;
        // INC
        case 6:
            *sp = *sp + current.m;
            break;
        // JMP
        case 7:
            *pc = current.m;
            break;
        // JPC
        case 8:
            if(registers[current.r] == 0 ){
                *pc = current.m;

            }
            break;
        // SIO (print)
        case 9:
            printf("%d\n", registers[current.r]);
            break;
        // SIO (read)
        case 10:
            printf("Input int: ");
            scanf("%d", &registers[current.r]);
            break;
        // SIO (HLT)
        case 11:
            *hlt = 1;
            break;
        // NEG
        case 12:
            registers[current.r] = registers[current.l]*-1;
            break;
        // ADD
        case 13:
            registers[current.r] = registers[current.l] + registers[current.m];
            break;
        // SUB
        case 14:
            registers[current.r] = registers[current.l] - registers[current.m];
            break;
        // MUL
        case 15:
            registers[current.r] = registers[current.l] * registers[current.m];
            break;
        // DIV
        case 16:
            registers[current.r] = registers[current.l] / registers[current.m];
            break;
        // ODD
        case 17:
            registers[current.r] = registers[current.r]%2;
            break;
        // MOD
        case 18:
            registers[current.r] = registers[current.l] % registers[current.m];
            break;
        // EQL
        case 19:
            registers[current.r] = registers[current.l] == registers[current.m];
            break;
        // NEQ
        case 20:
            registers[current.r] = registers[current.l] != registers[current.m];
            break;
        // LSS
        case 21:
            registers[current.r] = registers[current.l] < registers[current.m];
            break;
        // LEQ
        case 22:
            registers[current.r] = registers[current.l] <= registers[current.m];
            break;
        // GTR
        case 23:
            registers[current.r] = registers[current.l] > registers[current.m];
            break;
        // GEQ
        case 24:
            registers[current.r] = registers[current.l] >= registers[current.m];
            break;
        default:
            break;
    }
}

//
int base(int * stack,int l,int base){

    int b1;
    b1 = base;
    while (l > 0){
        b1 = stack[b1 + 1];
        l--;
    }

    return b1;
}

// Converts the op code to the relevant string
void traceCode(FILE * f, int line, instruction current){
	char instr[3];
	char space = ' ';
	int hasL = 0;

	switch(current.op){
	    // load literal
		case 1:
			strcpy(instr,"LIT");
			break;
        // return from subroutine
		case 2:
		    strcpy(instr,"RTN");
			break;
        // load value
        case 3:
            strcpy(instr,"LOD");
			break;
        // store value
        case 4:
            strcpy(instr,"STO");
			break;
        // call procedure
        case 5:
            strcpy(instr,"CAL");
			break;
        // increment stack pointer by m
        case 6:
            strcpy(instr,"INC");
			break;
        // jump to instruction m
        case 7:
            strcpy(instr,"JMP");
			break;
        // jump to instruction m if r = 0
        case 8:
            strcpy(instr,"JPC");
			break;
        case 9:
            strcpy(instr,"SIO");
			break;
        case 10:
            strcpy(instr,"SIO");
			break;
        case 11:
            strcpy(instr,"SIO");
			break;
        // -1*
        case 12:
            strcpy(instr,"NEG");
			break;
        // +
        case 13:
            strcpy(instr,"ADD");
			break;
        // -
        case 14:
            strcpy(instr,"SUB");
			break;
        // *
        case 15:
            strcpy(instr,"MUL");
			break;
        // /
        case 16:
            strcpy(instr,"DIV");
			break;
        // %2
        case 17:
            strcpy(instr,"ODD");
			break;
        // %
        case 18:
            strcpy(instr,"MOD");
			break;
        // ==
        case 19:
            strcpy(instr,"EQL");
			break;
        // !=
        case 20:
            strcpy(instr,"NEQ");
			break;
        // <
        case 21:
            strcpy(instr,"LSS");
			break;
        // <=
        case 22:
            strcpy(instr,"LEQ");
			break;
        // >
        case 23:
            strcpy(instr,"GTR");
			break;
        // >=
        case 24:
            strcpy(instr,"GEQ");
			break;
        default:
            printf("Faulty instruction\n");
            return;
	}
	fprintf(f, "%d\t%s\t%c\t%d\t%d\t%d", line, instr, space,current.r,current.l, current.m);
	return;
}
