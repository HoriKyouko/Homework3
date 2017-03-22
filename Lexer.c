#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_IDENT_LENGTH 11
#define MAX_NUM_LENGTH 5
#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_SYMBOLS 500

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym
} token_type;


// For constants, you must store kind, name and value.
// For variables, you must store kind, name, L and M.
// For procedures, you must store kind, name, L and M.

// I'm using this for all symbols and words.
typedef struct  {
	int kind; 		// symbol = 0, const = 1, var = 2, proc = 3, keyword = 4;
	int type;       // token_type
	char name[MAX_IDENT_LENGTH];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
} symbol;

void findType(symbol * sym, int * level,int *kindFlag);
int AddSymbol(char * name,int * numSym, symbol * sym_tab);
char * TextType(int type);

int main(){

    // I'm going to use the struct for every symbol
    symbol lex_table[MAX_SYMBOLS];
    // Separate array for just the identifiers (procedure, const, var)
    symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
    char code[MAX_CODE_LENGTH];

    FILE *pl0Code;
    FILE *lexOutput;
    FILE *symOutput;
    FILE *tokText;

    // initially refers the character form the code being recorded
    // then, to the current symbol being recorded
    int currentSymbol = 0;

    char currentChar = ' ';

    int isCommenting = 0;
    // Counter for the name array in the struct
    int namePos = 0;

    int lexLevel = 0;

    int kindFlag = 0;

    int numSymbols = 0;

    // error flags
    int flagNumLength = 0;
    int flagIdentLength = 0;
    int flagIdentInvalid = 0;
    int flagInvalidSym = 0;

    pl0Code = fopen("Input.txt", "r");

    while(!feof(pl0Code) && currentSymbol<MAX_SYMBOLS){

        currentChar = fgetc(pl0Code);

        if(currentChar == EOF){
            break;
        }

        if(isCommenting && currentChar == '*'){
            currentChar = fgetc(pl0Code);
            if(currentChar == EOF){
                break;
            }

            if(currentChar == '/'){
                isCommenting = 0;
                currentChar = fgetc(pl0Code);
            }


        }
        if(currentChar == '/' && !isCommenting){
            currentChar = fgetc(pl0Code);

            if(currentChar == EOF){
                break;
            }

            if(currentChar == '*'){
                isCommenting = 1;
            } else{
                code[currentSymbol] = '/';
                currentSymbol++;
            }
        }

        if(!isCommenting){
            code[currentSymbol] = currentChar;
            currentSymbol++;
        }

    }

    code[currentChar] = '\0';

    fclose(pl0Code);

    int codeLength = currentSymbol;

    currentSymbol = 0;
    currentChar = ' ';

    int i;

    for(i = 0; i<codeLength; i++){

        if(isalnum(code[i])){
               while(i < codeLength && isalnum(code[i])){
                    if(!isalpha(lex_table[currentSymbol].name[0]) && namePos > MAX_NUM_LENGTH){
                        flagNumLength = 1;
                    } else if(namePos >= MAX_IDENT_LENGTH){
                        flagIdentLength = 1;
                    }

                    lex_table[currentSymbol].name[namePos] = code[i];

                    if(isalpha(code[i]) && !isalpha(lex_table[currentSymbol].name[0]) ){
                        flagIdentInvalid = 1;
                        break;
                    }

                    namePos++;
                    i++;
               }
               if(flagIdentInvalid || flagIdentLength || flagNumLength){
                    break;
               }

               if(!isalpha(lex_table[currentSymbol].name[0])){
                    lex_table[currentSymbol].kind = 1;
                    lex_table[currentSymbol].type = numbersym;
                    lex_table[currentSymbol].val = atoi(lex_table[currentSymbol].name);
                    lex_table[currentSymbol].name[5] = '\0';

                    if(kindFlag == 1){
                        lex_table[currentSymbol-2].val = lex_table[currentSymbol].val;
                    }
               } else{
                    findType(&lex_table[currentSymbol], &lexLevel, &kindFlag);
               }

                if(lex_table[currentSymbol].kind != 4){
                    numSymbols = AddSymbol(lex_table[currentSymbol].name, &numSymbols, symbol_table);
                }

               if(namePos < MAX_IDENT_LENGTH-1){
                    lex_table[currentSymbol].name[namePos] = '\0';
               }
               namePos = 0;
               currentSymbol++;
        }
        if(code[i] != ' ' && code[i] != '\n' && code[i] != '\t' && code[i] != '\0' && code[i] != '\r'){
            // check special symbols
            if(!isalnum(code[i])){
                namePos = 0;

                lex_table[currentSymbol].name[namePos] = code[i];
                namePos++;
                switch(code[i]){
                case '+':
                    lex_table[currentSymbol].type = plussym;
                    break;
                case '-':
                    lex_table[currentSymbol].type = minussym;
                    break;
                case '*':
                    lex_table[currentSymbol].type = multsym;
                    break;
                case '/':
                    lex_table[currentSymbol].type= slashsym;
                    break;
                case '=':
                    lex_table[currentSymbol].type= eqsym;
                    break;
                case '<':
                    i++;
                    switch(code[i]){
                    case '=':
                        lex_table[currentSymbol].name[namePos] = code[i];
                        lex_table[currentSymbol].type = leqsym;
                        namePos++;
                        break;
                    case '>':
                        lex_table[currentSymbol].name[namePos] = code[i];
                        lex_table[currentSymbol].type = neqsym;
                        namePos++;
                        break;
                    default:
                        lex_table[currentSymbol].type = lessym;
                        i--;
                        break;
                    }
                case '>':
                    i++;
                    if(code[i]=='='){
                        lex_table[currentSymbol].name[namePos] = code[i];
                        lex_table[currentSymbol].type = geqsym;
                        namePos++;
                    } else{
                        lex_table[currentSymbol].type = gtrsym;
                        i--;
                    }
                    break;
                case '(':
                    lex_table[currentSymbol].type = lparentsym;
                    break;
                case ')':
                    lex_table[currentSymbol].type = rparentsym;
                    break;
                case ',':
                    lex_table[currentSymbol].type = commasym;
                    break;
                case ';':
                    lex_table[currentSymbol].type = semicolonsym;
                    kindFlag = 0;
                    break;
                case '.':
                    lex_table[currentSymbol].type = periodsym;
                    break;
                case ':':
                    i++;
                    if(code[i] == '='){
                        lex_table[currentSymbol].name[namePos] = code[i];
                        lex_table[currentSymbol].type = becomessym;
                        namePos++;
                    } else{
                        flagInvalidSym = 1;
                    }
                    break;
                default:
                    flagInvalidSym = 1;
                    break;
                }

                if(flagInvalidSym){
                    break;
                }

                lex_table[currentSymbol].name[namePos] = '\0';
                lex_table[currentSymbol].kind = 0;
                currentSymbol++;
                namePos = 0;
            }
        }

    }

    lexOutput = fopen("lexList.txt", "w");
    // NEED TO REPLACE RETURN 0 WITH ACTUAL ERROR HANDELING
    if(flagIdentInvalid){
        printf("ERROR: Invalid identifier: %s; identifier must start with a letter", lex_table[currentSymbol].name);
        fprintf(lexOutput,"ERROR: Invalid identifier: %s; identifier must start with a letter", lex_table[currentSymbol].name);
        return 0;
    } else if(flagIdentLength){
        printf("ERROR: Invalid identifier; length of %s exceeds %d characters\n", lex_table[currentSymbol].name, MAX_IDENT_LENGTH);
        fprintf(lexOutput, "ERROR: Invalid identifier; length of %s exceeds %d characters\n", lex_table[currentSymbol].name, MAX_IDENT_LENGTH);
        return 0;
    } else if(flagInvalidSym){
        printf("ERROR: Invalid symbol: %s\n", lex_table[currentSymbol].name);
        fprintf(lexOutput, "ERROR: Invalid symbol: %s\n", lex_table[currentSymbol].name);
        return 0;
    } else if(flagNumLength){
        printf("ERROR: Invalid number; length exceeds %d digits.\n", MAX_NUM_LENGTH);
        fprintf(lexOutput,"ERROR: Invalid number; length exceeds %d digits.\n", MAX_NUM_LENGTH);
        return 0;
    }

    for(i=0; i<currentSymbol; i++){
        if(lex_table[i].type == identsym || lex_table[i].type == numbersym){
            fprintf(lexOutput, "%d %s ", lex_table[i].type, lex_table[i].name);
        }else{
            fprintf(lexOutput, "%d ", lex_table[i].type);
        }
    }

    fclose(lexOutput);

    // print symbol table
    symOutput = fopen("symbolList.txt", "w");

    for(i = 0; i<numSymbols; i++){
        fprintf(symOutput, "%s ", symbol_table[i].name);
    }

    fclose(symOutput);

    tokText = fopen("tokenTypeText.txt", "w");
    // print lex list in text
    for(i=0; i<currentSymbol; i++){
        if(lex_table[i].type == identsym || lex_table[i].type == numbersym){
            fprintf(tokText, "%s %s ", TextType(lex_table[i].type), lex_table[i].name);
        }else{
            fprintf(tokText, "%s ", TextType(lex_table[i].type));
        }
    }

    fclose(tokText);


    return 0;
}

void findType(symbol * sym, int * level, int *kindFlag){
    if(!strcmp(sym->name,"begin")){
        sym->type = beginsym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "end")){
        sym->type = endsym;
        sym->kind = 4;
        *level = *level-1;
    } else if(!strcmp(sym->name, "if")){
        sym->type = ifsym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "then")){
        sym->type = thensym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "while")){
        sym->type = whilesym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "do")){
        sym->type = dosym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "call")){
        sym->type = callsym;
        sym->kind = 4;
        *level = *level+1;
    } else if(!strcmp(sym->name, "const")){
        sym->type = constsym;
        *kindFlag = 1;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "var")){
        sym->type = varsym;
        *kindFlag = 2;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "procedure")){
        sym->type = procsym;
        *kindFlag = 3;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "write")){
        sym->type = writesym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "read")){
        sym->type = readsym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "else")){
        sym->type = elsesym;
        sym->kind = 4;
    } else if(!strcmp(sym->name, "odd")){
        sym->type = oddsym;
        sym->kind = 4;
    } else{
        sym->type = identsym;
        sym->level = *level;
        if(*kindFlag!= 0){
           sym->kind = *kindFlag;
           if(*kindFlag == 2 || *kindFlag == 3){
                sym->level = *level;
           }
        }
    }

}

int AddSymbol(char * name,int * numSym, symbol * sym_tab){
    int i = 0;

    for(i; i<*numSym; i++){
        if(!strcmp(sym_tab[i].name, name)){
            return *numSym;
        }
    }

    strcpy(sym_tab[i].name, name);
    return i+1;
}

char * TextType(int type){
    char* typeName = malloc(14);
    switch(type){
    case 1:
        strcpy(typeName, "nulsym");
        break;
    case 2:
        strcpy(typeName, "identsym");
        break;
    case 3:
        strcpy(typeName, "numbersym");
        break;
    case 4:
        strcpy(typeName, "plussym");
        break;
    case 5:
        strcpy(typeName, "minussym");
        break;
    case 6:
        strcpy(typeName, "multsym");
        break;
    case 7:
        strcpy(typeName, "slashsym");
        break;
    case 8:
        strcpy(typeName, "oddsym");
        break;
    case 9:
        strcpy(typeName, "eqsym");
        break;
    case 10:
        strcpy(typeName, "neqsym");
        break;
    case 11:
        strcpy(typeName, "lessym");
        break;
    case 12:
        strcpy(typeName, "leqsym");
        break;
    case 13:
        strcpy(typeName, "gtrsym");
        break;
    case 14:
        strcpy(typeName, "geqsym");
        break;
    case 15:
        strcpy(typeName, "lparentsym");
        break;
    case 16:
        strcpy(typeName, "rparentsym");
        break;
    case 17:
        strcpy(typeName, "commasym");
        break;
    case 18:
        strcpy(typeName, "semicolonsym");
        break;
    case 19:
        strcpy(typeName, "periodsym");
        break;
    case 20:
        strcpy(typeName, "becomessym");
        break;
    case 21:
        strcpy(typeName, "beginsym");
        break;
    case 22:
        strcpy(typeName, "endsym");
        break;
    case 23:
        strcpy(typeName, "ifsym");
        break;
    case 24:
        strcpy(typeName, "thensym");
        break;
    case 25:
        strcpy(typeName, "whilesym");
        break;
    case 26:
        strcpy(typeName, "dosym");
        break;
    case 27:
        strcpy(typeName, "callsym");
        break;
    case 28:
        strcpy(typeName, "constsym");
        break;
    case 29:
        strcpy(typeName, "varsym");
        break;
    case 30:
        strcpy(typeName, "procsym");
        break;
    case 31:
        strcpy(typeName, "writesym");
        break;
    case 32:
        strcpy(typeName, "readsym");
        break;
    case 33:
        strcpy(typeName, "elsesym");
        break;
    default:
        strcpy(typeName, "invalidsym");
        break;
    }

    return typeName;
}
