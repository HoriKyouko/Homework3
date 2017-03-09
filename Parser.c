#include <stdio.h>
#include <string.h>

#define MAX_token_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500

typedef struct{

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

} op_type

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
int curReg;
token tokens[MAX_token_TABLE_SIZE];
int tokenTableIndex;
instruction code[MAX_CODE_LENGTH];
int level;
int codeIndex;

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

node *createNode(int data);
node *insertNode(node *head, node *tail, int token);
node *getLexemeList();
int getTokenList(token *tokenList);

void emit(int op, int r, int l, int m);



int main(int argc, char **argv){

    int currentElement, symbolSize;

    node *currentNode, **head;
    currentNode = getLexemeList();

    symbolSize = getTokenList(tokens);
    // Stuff from Compiler Driver goes here
    Program(currentNode);
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
    if(currentToken == constsym)
        ConstDecl(currentNode);

    if(currentToken == varsym)
        VarDecl(currentNode);

    if(currentToken ==  procsym)
        ProcDecl(currentNode);

    Statement(currentNode);
}

int ConstDecl(node *currentNode){
    int count = 0;

    do{
        getNextToken(currentNode);
        if(currentToken != identsym)
            error(4);

        count++;
        getNextToken(currentNode);

        if(currentToken != eqsym){
            if(currentToken == becomessym)
                error(1);
            else
                error(3);
        }

        getNextToken(currentNode);

        if(currentToken != numsym)
            error(2);

        getNextToken(currentNode);

    } while(currentToken == commasym);

    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);

    return count;
}

int VarDecl(node *currentNode){
    int count = 0;
    int tokenIndex;

    do{
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(4);

        tokenIndex = currentToken;
        addSymbolTable(variable, tokenIndex);
        tokens[tokenTableIndex].addr = 4 + varCount;

        getNextToken(currentNode);

        count++;
    } while(currentToken == commasym);

    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);

    return count;
}

int ProcDecl(node *currentNode){
    int count = 0;
    int tokenIndex;

    while(currentToken == procsym){
        count++;
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(4);

        getNextToken(currentNode);

        tokenIndex = currentToken;
        addSymbolTable(procedure, tokenIndex);
        tokens[tokenTableIndex].level = level;
        tokens[tokenTableIndex].addr = codeIndex;

        if(currentToken != semicolonsym)
            error(5);

        getNextToken(currentNode);
        Block(currentNode);

        if(tokens[*currentElement].type!=semicolonsym)
            error(5);

        getNextToken(currentNode);
    }
    return count;
}

void Statement(node *currentNode){

    if(currentToken == identsym){
        getNextToken(currentNode);

        i = currentToken

        index = findToken(i);

        if(index == 0)
            error(11);

        if(tokenTable[index].kind != variable)
            error(12);

        getNextToken(currentNode);

        if(currentToken != becomessym)
            error(13);

        getNextToken(currentNode);
        Expression(currentNode);
        emit(op_sto, curReg, level - tokens[index].level, tokens[index].addr);
        curReg--;
    }// end ident

    else if(currentToken == callsym){
        getNextToken(currentNode);

        if(currentToken != identsym)
            error(14);

        getNextToken(currentNode);
    }// end call

    else if (currentToken == beginsym]){
        getNextToken(currentNode);
        Statement(currentNode)
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
        // More stuff goes here

        /*
        if(tokens[*currentElement].type == elsesym){
            // Stuff goes here as well
        }
        */

        getNextToken(currentNode);
        Statement(currentNode)
    }// end if

    else if(currentToken == whilesym){
        getNextToken(currentNode);
        Condition(currentNode);

        if(currentToken != dosym)
            error(18);

        getNextToken(currentNode);
        Statement(currentNode);
    }// end while
}

void Condition(node *currentNode){
    int op;
    if(currentNode == oddsym){
        getNextToken(currentNode);;

        Expression(currentNode);

        emit(op_odd, curReg, 0, 0);
    }

    else{
        Expression(currentNode);

        op = rel_op();

        if(!rel_op())
            error(13);

        getNextToken(currentNode);

        Expression(currentNode);

        emit(op, curReg - 1, curReg - 1, curReg);
        curReg--;
    }
}

int rel_op(){
    // Should be a switch statement I feel.
    switch(){

        case eqsym:
            return op_eql;
            break;

        case neqsym:
            return op_neq;
            break;

        case lessym:
            return op_lss;
            break;

        case leqsym:
            return op_leq;
            break;

        case gtrsym:
            return op_gtr;
            break;

        case geqsym:
            return op_geq;
            break;

        default:
            return 0;
    }
}

void Expression(node *currentNode){
    int addOp;

    if(currentToken == plussym || currentToken == minussym){
        addOp = currentToken;
        getNextToken(currentNode);
        Term(currentNode);
        // need to figure out r, l , and m
        emit(op_neg, curReg, curReg, 0);
    }
    else
        Term(currentNode);

    while(currentToken == plussym || currentToken == minussym){
        addOp = currentToken;
        getNextToken(currentNode);
        Term(currentNode);
        if(addOp == plussym){
            emit(op_add, curReg - 1, curReg - 1, curReg);
            curReg--;
        }
        else if(addOp == minussym){
            emit(op_sub, curReg - 1, curReg - 1, curReg);
            curReg--;
        }
    }
}

void Term(node *currentNode){
    int mulOP;
    Factor(currentNode);
    getNextToken(currentNode);
    while(currentToken == multsym || currentToken == slashsym){
        mulOp = currentToken;
        getNextToken(currentNode);
        Factor(currentNode)
    }
}

void Factor(node *currentNode){

    if(currentToken == identsym){
        getNextToken(currentNode);
        // Some more code
        // If the symbol table kind doesn't equal variable or constant throw error(14)
    }

    else if(currentToken == numbersym){
        getNextToken(currentNode);
    }

    else if(currentToken == lparentsym){
        getNextToken(currentNode);
        Expression(currentNode);
        if(currentToken != rparentsym){
            error(22);
        }
        getNextToken(currentNode);
    }

    else{
        error(24);
    }
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
            printf("14. call must be followed by an identifier.");
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

        case 26:
            printf("26. Unexpected symbol.");
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

/* For binary files
    fp = fopen("lexList.txt", "rb");
*/
    fp = fopen("lexList.txt", "r");

    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }
    node *head, *tail;

    head = tail = NULL;

    fscanf(fp, " %d", &buffer);

    head = tail = insertNode(head, tail, buffer);

    while(fscanf(fp, "%d", buffer) != EOF){
        tail = insertNode(head, tail, buffer);
        tail->token = buffer;
    }

    fclose(fp);

    return head;
}

int getTokenList(token *tokenList){
    char buffer[11];
    int i = 0;

    FILE fp;

    fp = fopen("symTable.txt", "r");

    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }

    while(fscanf(fp, "%s", buffer) != EOF){
        strcpy(tokenList[i].name, buffer);
        i++;
    }

    fclsoe(fp);
    return i;
}

void getNextToken(node *currentNode){

    currentToken = currentNode->token;

    if(currentNode->next != NULL)
        *currentNode = *currentNode->next;
}
