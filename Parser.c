#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_token_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500
#define MAX_IDENT_LENGTH 11

typedef struct node{

    int token;
    struct node *next;

}node;

typedef enum {

    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym

} token_type;

typedef enum {

    op_lit = 1, op_rtn, op_lod, op_sto, op_cal, op_inc, op_jmp, op_jpc,
    op_siop, op_sior, op_sioh, op_neg, op_add, op_sub, op_mul, op_div,
    op_odd, op_mod, op_eql, op_neq, op_lss, op_leq, op_gtr, op_geq

} op_type;

typedef struct  {

    int kind; 		// symbol = 0, const = 1, var = 2, proc = 3, keyword = 4;
    char name[MAX_IDENT_LENGTH];	// name up to 11 chars
    int val; 		// number (ASCII value)
    int level; 		// L level
    int addr; 		// M address

} token;

typedef struct {

    int op;		// op code
    int r;      // register
    int l;   	// level
    int m;   	// offset

} instruction;

typedef enum{

    constant = 1,
    variable,
    procedure

}symbolKind;

// Global Variables needed
int currentToken;
int currentReg;
token tokenTable[MAX_token_TABLE_SIZE]; // Token Table
token tokens[MAX_token_TABLE_SIZE]; // Token List
int tokenTableIndex; // Tells us what token we are looking at.
instruction code[MAX_CODE_LENGTH];
int level;
int codeIndex; // Tells what line we are in the code.


// Function declarations
void Program(node *currentNode);
void Block(node *currentNode);
int ConstDecl(node *currentNode);
int VarDecl(node *currentNode);
int ProcDecl(node *currentNode);
void Statement(node *currentNode);
void Condition(node *currentNode);
int rel_op();
void Expression(node *currentNode);
void Term(node *currentNode);
void Factor(node *currentNode);
void getNextToken(node *currentNode);
void error(int error);
void emit(int op, int r, int l, int m);
void textForVM()

node *createNode(int data);
node *insertNode(node *head, node *tail, int token);

node *getLexemeList();
int getTokenList(token *tokenList);
void addSymbolTable(int kind, int tokenIndex);
int findToken(int token);

int main(int argc, char **argv){
    int currentElement, tokenSize;

    currentReg = -1;
    codeIndex = 0;
    level = -1;
    tokenTableIndex = 0;
    node *currentNode, **head;

    currentNode = getLexemeList();

    tokenSize = getTokenList(tokens);
    // Stuff from Compiler Driver goes here
    Program(currentNode);

    textForVM();
}

void Program(node *currentNode){
    getNextToken(currentNode);;

    Block(currentNode);

    if(currentToken != periodsym)
        error(9);

    else
        emit(op_sioh, 0, 0, 3);
}

void Block(node *currentNode){
    int space, variables = 0, procedures = 0, constants = 0, jump;

    level++;

    space = 4;

    jump = codeIndex;
    emit(op_jmp, 0, 0 ,0);

    if(currentToken == constsym)
        constants = ConstDecl(currentNode);

    if(currentToken == varsym)
        variables = VarDecl(currentNode);

    space += variables;

    if(currentToken ==  procsym)
        procedures = ProcDecl(currentNode);

    code[jump].m = codeIndex;
    emit(op_inc, 0, 0, space);

    Statement(currentNode);
    tokenTableIndex = tokenTableIndex - (variables + procedures + constants);

    emit(op_rtn, 0, 0, 0);
    level--;
}

int ConstDecl(node *currentNode){
    int count = 0, index, value;

    do{
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(4);

        getNextToken(currentNode);
        index = currentToken;
        addSymbolTable(constant, index);
        count++;

        getNextToken(currentNode);

        if(currentToken != eqsym){
            if(currentToken == becomessym)
                error(1);
            else
                error(3);
        }

        getNextToken(currentNode);

        if(currentToken != numbersym)
            error(2);

        getNextToken(currentNode);

        index = currentToken;
        value = atoi(tokens[index].name);
        tokenTable[tokenTableIndex].val = value;

        getNextToken(currentNode);

    } while(currentToken == commasym);

    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);

    return count;
}

int VarDecl(node *currentNode){
    int count = 0, index;

    do{
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(4);

        getNextToken(currentNode);

        index = currentToken;
        addSymbolTable(variable, index);
        tokenTable[tokenTableIndex].addr = 4 + count;

        getNextToken(currentNode);

        count++;
    } while(currentToken == commasym);

    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);

    return count;
}

int ProcDecl(node *currentNode){
    int count = 0, index;

    while(currentToken == procsym){
        count++;
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(4);

        getNextToken(currentNode);

        index = currentToken;
        addSymbolTable(procedure, index);
        tokenTable[tokenTableIndex].level = level;
        tokenTable[tokenTableIndex].addr = codeIndex;

        getNextToken(currentNode);

        if(currentToken != semicolonsym)
            error(5);

        getNextToken(currentNode);
        Block(currentNode);

        if(currentToken != semicolonsym)
            error(5);

        getNextToken(currentNode);
    }
    return count;
}

void Statement(node *currentNode){
    int temp, codeTemp, index, indexTemp;

    if(currentToken == identsym){
        getNextToken(currentNode);

        index = findToken(currentToken);

        if(index == 0)
            error(11);

        if(tokenTable[index].kind != variable)
            error(12);

        getNextToken(currentNode);

        if(currentToken != becomessym)
            error(13);

        getNextToken(currentNode);
        Expression(currentNode);
        emit(op_sto, currentReg, level - tokenTable[index].level, tokenTable[index].addr);
        currentReg--;
    }// end ident

    else if(currentToken == callsym){
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(14);

        getNextToken(currentNode);

        index = findToken(currentToken);

        if(index == 0)
            error(11);

        else if(tokenTable[index].kind != procedure)
            error(15);

        emit(op_cal, 0, level - tokenTable[index].level, tokenTable[index].addr);

        getNextToken(currentNode);
    }// end call

    else if (currentToken == beginsym){

        getNextToken(currentNode);
        Statement(currentNode);

        while(currentToken == semicolonsym){
            getNextToken(currentNode);
            Statement(currentNode);
        }

        if(currentToken != endsym)
            error(17);

        getNextToken(currentNode);
    }// end begin

    else if(currentToken == ifsym){

        getNextToken(currentNode);
        Condition(currentNode);

        if(currentToken != thensym)
            error(16);

        getNextToken(currentNode);

        temp = codeIndex;
        emit(op_jpc, currentReg, 0, 0);
        currentReg--;

        Statement(currentNode);

        if(currentToken == elsesym){
            getNextToken(currentNode);

            codeTemp = codeIndex;
            emit(op_jmp, 0, 0,0);

            code[temp].m = codeIndex;

            Statement(currentNode);

            code[codeTemp].m = codeIndex;
        }
        else
            code[temp].m = codeIndex;

    }// end if

    else if(currentToken == whilesym){

        indexTemp = codeIndex;

        getNextToken(currentNode);
        Condition(currentNode);
        codeTemp = codeIndex;

        emit(op_jpc, currentReg, 0, 0);

        if(currentToken != dosym)
            error(18);

        getNextToken(currentNode);
        Statement(currentNode);

        emit(op_jmp, 0, 0, indexTemp);

        code[codeTemp].m = codeIndex;
    }// end while
}

void Condition(node *currentNode){
    int op;

    if(currentToken == oddsym){
        getNextToken(currentNode);;

        Expression(currentNode);

        emit(op_odd, currentReg, 0, 0);
    }

    else{
        Expression(currentNode);

        op = rel_op();

        if(!rel_op())
            error(20);

        getNextToken(currentNode);

        Expression(currentNode);

        emit(op, currentReg - 1, currentReg - 1, currentReg);
        currentReg--;
    }
}

int rel_op(){
    switch(currentToken){

        case eqsym:
            return op_eql;

        case neqsym:
            return op_neq;

        case lessym:
            return op_lss;

        case leqsym:
            return op_leq;

        case gtrsym:
            return op_gtr;

        case geqsym:
            return op_geq;

        default:
            return 0;
    }
}

void Expression(node *currentNode){
    int op;

    if(currentToken == plussym || currentToken == minussym){

        getNextToken(currentNode);
        Term(currentNode);
        emit(op_neg, currentReg, currentReg, 0);

    }
    else
        Term(currentNode);

    while(currentToken == plussym || currentToken == minussym){

        op = currentToken;
        getNextToken(currentNode);
        Term(currentNode);

        if(op == plussym){
            emit(op_add, currentReg - 1, currentReg - 1, currentReg);
            currentReg--;
        }
        else if(op == minussym){
            emit(op_sub, currentReg - 1, currentReg - 1, currentReg);
            currentReg--;
        }
    }
}

void Term(node *currentNode){
    int op;

    Factor(currentNode);

    while(currentToken == multsym || currentToken == slashsym){

        op = currentToken;
        getNextToken(currentNode);
        Factor(currentNode);

        if(op == multsym){
            emit(op_mul, currentReg - 1, currentReg - 1, currentReg);
            currentReg--;
        }
        else if(op == slashsym){
            emit(op_div, currentReg - 1, currentReg - 1, currentReg);
            currentReg--;
        }
    }
}

void Factor(node *currentNode){
    int index, value;

    if(currentToken == identsym){
        getNextToken(currentNode);

        index = findToken(currentToken);
        currentReg++;

        if(tokenTable[index].kind == variable)
            emit(op_lod, currentReg, level - tokenTable[index].level, tokenTable[index].addr);

        else if(tokenTable[index].kind == constant)
            emit(op_lit, currentReg, 0, tokenTable[index].val);

        else
            error(21);

        getNextToken(currentNode);
    }

    else if(currentToken == numbersym){
        getNextToken(currentNode);

        value = atoi(tokens[currentToken].name);
        currentReg++;

        emit(op_lit, currentReg, 0, value);
        getNextToken(currentNode);
    }

    else if(currentToken == lparentsym){

        getNextToken(currentNode);
        Expression(currentNode);

        if(currentToken != rparentsym)
            error(22);

        getNextToken(currentNode);
    }

    else
        error(24);
}

void emit(int op, int r, int l, int m){

    if(codeIndex > MAX_CODE_LENGTH)
        error(25);

    else{
        code[codeIndex].op = op;
        code[codeIndex].r = r;
        code[codeIndex].l = l;
        code[codeIndex].m = m;
        codeIndex++;
    }
}

void error(int error){
    printf("Error: ");
    switch(error){

        case 1:
            printf("1. Use = instead of :=.");
            break;

        case 2:
            printf("2. = must be followed by a number.");
            break;

        case 3:
            printf("3. Identifier must be followed by =.");
            break;

        case 4:
            printf("4. const, var, procedure must be followed by identifier.");
            break;

        case 5:
            printf("5. Semicolon or comma missing.");
            break;

        case 6:
            printf("6. Incorrect symbol after procedure declaration");
            break;

        case 7:
            printf("7. Statement expected");
            break;

        case 8:
            printf("8. Incorrect symbol after statement part in block.");
            break;

        case 9:
            printf("9. Period expected.");
            break;

        case 10:
            printf("10. Semicolon between statements missing.");
            break;

        case 11:
            printf("11. Undeclared identifier");
            break;

        case 12:
            printf("12. Assignment to constant or procedure is not allowed");
            break;

        case 13:
            printf("13. Assignment operator expected");
            break;

        case 14:
            printf("14. Call must be followed by an identifier.");
            break;

        case 15:
            printf("15. Call of a constant or variable is meaningless.");
            break;

        case 16:
            printf("16. then expected.");
            break;

        case 17:
            printf("17. Semicolon or } expected.");
            break;

        case 18:
            printf("18. do expected.");
            break;

        case 19:
            printf("19. Incorrect symbol following statement.");
            break;

        case 20:
            printf("20. Relational operator expected.");
            break;

        case 21:
            printf("21. Expression must not contain a procedure identifier.");
            break;

        case 22:
            printf("22. Right parenthesis missing.");
            break;

        case 23:
            printf("23. The preceding factor cannot begin with this symbol.");
            break;

        case 24:
            printf("24. An expression cannot begin with this symbol.");
            break;

        case 25:
            printf("25. This number is too large.");
            break;

        default:
            printf("Unknown error");
            break;
    }
    printf("\n");
    exit(1);
}

node* createNode(int data){
    node *ptr = (node*)malloc(sizeof(node));

    ptr->token = data;
    ptr->next = NULL;

    return ptr;
}

node* insertNode(node *head, node *tail, int token){
    if(head == NULL)
        return createNode(token);

    else{
        tail->next = createNode(token);
        return tail->next;
    }
}
node *getLexemeList(){
    int buffer;
    FILE *fp;
    node *head, *tail;

/* For binary files
    fp = fopen("lexList.txt", "rb");
*/
    fp = fopen("lexList.txt", "r");

    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }

    head = tail = NULL;

    fscanf(fp, " %d", &buffer);

    head = tail = insertNode(head, tail, buffer);

    while(fscanf(fp, "%d", &buffer) != EOF){
        tail = insertNode(head, tail, buffer);
        tail->token = buffer;
    }

    fclose(fp);

    return head;
}

int getTokenList(token *tokenList){
    char buffer[MAX_IDENT_LENGTH + 1];
    int count = 0;

    FILE *fp;

    fp = fopen("symTable.txt", "r");

    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }

    while(fscanf(fp, "%s", buffer) != EOF){
        strcpy(tokenList[count].name, buffer);
        count++;
    }

    fclose(fp);
    return count;
}

void getNextToken(node *currentNode){

    currentToken = currentNode->token;

    if(currentNode->next != NULL)
        *currentNode = *currentNode->next;
}

void addSymbolTable(int kind, int tokenIndex){
    tokenTableIndex++;

    strcpy(tokenTable[tokenTableIndex].name, tokens[tokenIndex].name);

    tokenTable[tokenTableIndex].level = level;
    tokenTable[tokenTableIndex].kind = kind;
}

int findToken(int token){
    int location;

    for(location = tokenTableIndex; location > 0; location--)
        if(strcmp(tokenTable[location].name, tokens[token].name) == 0)
            return location;

    return location;
}

void textForVM(){
    int i = 0;
    FILE *fp;

    fp = fopen("CodeGenOutput.txt", "w");

    for(i = 0; i < codeIndex; i++)
        fprintf(fp, "%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);

    fclose(fp);
}
