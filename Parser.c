// Parser for pl0 compiler
// Timothy Garrett
// James Williamson

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_token_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500
#define MAX_IDENT_LENGTH 11

// Each individual node holds one of the tokens in a link list fashion.
typedef struct node{

    int token;
    struct node *next;

} node;
// Values for our tokens.
typedef enum {

    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym

} token_type;
// The op values we will use to help generate our code in the VM.
typedef enum {

    op_lit = 1, op_rtn, op_lod, op_sto, op_cal, op_inc, op_jmp, op_jpc,
    op_siop, op_sior, op_sioh, op_neg, op_add, op_sub, op_mul, op_div,
    op_odd, op_mod, op_eql, op_neq, op_lss, op_leq, op_gtr, op_geq

} op_type;
// Each token contains the values that will be used to determine what kind of code will be generated.
typedef struct  {

    int kind; 		// symbol = 0, const = 1, var = 2, proc = 3
    char name[MAX_IDENT_LENGTH];	// name up to 11 chars
    int val; 		// number (ASCII value)
    int level; 		// L level
    int addr; 		// M address

} token;
// Once the code has been generated we will then create a reference to it in our instructions.
typedef struct {

    int op;		// op code
    int r;      // register
    int l;   	// level
    int m;   	// offset

} instruction;
// Determines the kind of token we are dealing wtih
typedef enum{

    constant = 1,
    variable,
    procedure

} kindOfToken;

// Global Variables needed
int currentToken;
int currentRegister;
token tokenTable[MAX_token_TABLE_SIZE];
token tokens[MAX_token_TABLE_SIZE];
int tokenTableIndex;
instruction code[MAX_CODE_LENGTH];
int level;
int codeIndex;
int varCounter;
int errorFlag;
int tokenNum;

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
void textForVM();

node *createNode(int data);
node *insertNode(node *head, node *tail, int token);

node *getLexList();
void getTokenList(token *tokenList);
void addTokenTable(int kind, int tokenIndex);
int findToken(int token);

int main(int argc, char **argv){
    int currentElement, tokenSize;
    // Initializing our values and creating our nodes.
    currentRegister = -1;
    codeIndex = 0;
    tokenNum = 0;
    level = -1;
    tokenTableIndex = 0;
    node *currentNode;
    // Populates our tokens array with the symbols.
    getTokenList(tokens);
    // Creates our LinkedList from the Lexer programs output.
    currentNode = getLexList();
    // Call our Program procedure.
    Program(currentNode);
    // Prints out our code we generated.
    textForVM();
}
// The Program procedure for our Parser.
void Program(node *currentNode){
    // Gets our next token in the LinkedList
    getNextToken(currentNode);

    Block(currentNode);
    // If our code does not have a period at the last end we throw an error
    if(currentToken != periodsym)
        error(9);
    // We add the halt condition via the emit function.
    else
        emit(op_sioh, 0, 0, 3);
    // If we get here  and errorFlag == 0 then we had no errors.
	if(errorFlag != 1)
		printf("No errors, program is syntactically correct.\n\n");
}
// The Block procedure for our Parser.
void Block(node *currentNode){
    int space = 4, variables = 0, procedures = 0, constants = 0, jump;
    // We increment the level since we have entered another level of our PL/0 code.
    level++;
    // We jump to the line of Code that we are currently at, and add the instruction to the emit function.
    jump = codeIndex;
    emit(op_jmp, 0, 0 ,0);
    // We then determine which procedure we go into next by determining the symbol given for the token.
    if(currentToken == constsym)
        constants = ConstDecl(currentNode);

    if(currentToken == varsym)
        variables = VarDecl(currentNode);
    // We add space determined by how many variables we have added.
    space += variables;

	if(currentToken == procsym)
		procedures = ProcDecl(currentNode);
    // We then make the code offset set to the line we are working on in the code, and call the emit function for
    // the INC call.
    code[jump].m = codeIndex;
    emit(op_inc, 0, 0, space);
    // Call the Statement procedure and find the Index of our token table we are at. This is only called when we
    // have another procedure and are returning from that procedure.
    Statement(currentNode);
    tokenTableIndex = tokenTableIndex - (variables + procedures + constants);
    // We then generate the code for Return and subtract a level we are on.
    emit(op_rtn, 0, 0, 0);
    level--;
}
// The Constant Declaration procedure
int ConstDecl(node *currentNode){
    int count = 0, pointer, value;
    // Keeps looping until we finally run out of commas or get some other token that is not a comma
    do{
        getNextToken(currentNode);
        // Check if our token is an identifier if it is not then we throw an error.
        if(currentToken != identsym)
            error(4);
        // Get the next token and then adds our token to the tokenTable and declare its kind as a constant.
        // We then increment our counter for how many constants are being added.
        getNextToken(currentNode);
        pointer = currentToken;
        addTokenTable(constant, pointer);
        count++;
        getNextToken(currentNode);
        // If our token isn't an equal symbol we then check if it is a becomes symbol and if it is we throw an error,
        // but if it isn't then we still throw an error but its a different error.
        if(currentToken != eqsym){
            if(currentToken == becomessym)
                error(27);
            else
                error(3);
        }

        getNextToken(currentNode);
        // If our token is not a number we then throw an error.
        if(currentToken != numbersym)
            error(2);
        // We then get our next token and determine its value and add it to the tokenTable at the location determined
        // by tokenTableIndex and specifically give the value in the val integer of our struct.
        getNextToken(currentNode);
        pointer = currentToken;
        value = atoi(tokens[pointer].name);
        tokenTable[tokenTableIndex].val = value;
        getNextToken(currentNode);

    } while(currentToken == commasym);
    // If we find our symbol is not a semicolon we then throw an error
    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);
    // We return the number of constants we have.
    return count;
}
// The Variable Declaration procedure.
int VarDecl(node *currentNode){
    int count = 0, pointer;
    // Keep looping until we have no more commas.
    do{
        getNextToken(currentNode);
        // If our token is not an identifier we throw an error.
        if(currentToken != identsym)
            error(4);
        // We then add our token to the tokenTable with the kind being a variable. The address of the variable is
        // determined by 4 + however many variables we have, because we must save space for other required stack calls
        // that come with a procedure. We also increment our counter for how many variables we have.
        getNextToken(currentNode);
        pointer = currentToken;
        addTokenTable(variable, pointer);
        tokenTable[tokenTableIndex].addr = 4 + count;
        getNextToken(currentNode);
        count++;

    } while(currentToken == commasym);
    // If our symbol is not a semicolon call an error.
    if(currentToken != semicolonsym)
        error(5);

    getNextToken(currentNode);
    // Return how many variables we have.
    return count;
}
// The Procedure Declaration Variable.
int ProcDecl(node *currentNode){
    int count = 0, pointer;

    do{
        getNextToken(currentNode);
        // If our token is not an identifier call an error.
        if(currentToken != identsym)
            error(4);

        getNextToken(currentNode);
		// Pointer gets currentToken and we add to the tokenTable procedure value and pointer value.
        pointer = currentToken;
        addTokenTable(procedure, pointer);
		// We then set the level at the tokenTableIndex to level,
		// and set the addr at the tokenTableIndex to be the codeIndex.
        tokenTable[tokenTableIndex].level = level;
        tokenTable[tokenTableIndex].addr = codeIndex;
        getNextToken(currentNode);
		// Increment count; for how many procedures we have.
        count++;
        // If our token is not a semicolon we throw an error.
        if(currentToken != semicolonsym)
            error(5);
        // We then call for our next token in the linked list and then call our Block function since, we are calling
        // a procedure and this procedure may have constants, variables, procedures, or statements in it.
        getNextToken(currentNode);
        Block(currentNode);
        // Token isn't a semicolon throw an error.
        if(currentToken != semicolonsym)
            error(5);

        getNextToken(currentNode);
    } while(currentToken == procsym);
    // Return how many procedures we called.
    return count;
}
// The Statement procedure.
void Statement(node *currentNode){
    int temp, codeTemp, pointer, pointerTemp;
    // We check whether our token is an identifier symbol.
    if(currentToken == identsym){
        getNextToken(currentNode);
        // We then find our tokens position in our tokenTable and store it in pointer.
        pointer = findToken(currentToken);
        // If our pointer equals zero then we throw an error.
        if(pointer == 0)
            error(11);
        // If our position in the tokenTables kind isn't a variable we throw an error.
        if(tokenTable[pointer].kind != variable)
            error(12);

        getNextToken(currentNode);
        // If our token is not a becomessym we throw an error.
        if(currentToken != becomessym){
            if(currentToken == eqsym){
                error(1);
            } else{
                error(13);
            }
        }
        // We get our next token and call the Expression procedure. Once were done with the Expression Procedure we then
        // output our Store command via the emit function. We also free up the register we were currently using.
        getNextToken(currentNode);
        Expression(currentNode);
        emit(op_sto, currentRegister, level - tokenTable[pointer].level, tokenTable[pointer].addr);
        currentRegister--;
    }
    // If our token is a call symbol.
    else if(currentToken == callsym){
        getNextToken(currentNode);
        // If our token is not an identifier throw an error.
        if(currentToken != identsym)
            error(14);
        // We get our token and find its position and store it in pointer.
        getNextToken(currentNode);
        pointer = findToken(currentToken);
        // If it equals 0 we then throw an error.
        if(pointer == 0)
            error(11);
        // If our position in the tokenTables kind isn't a procedure we throw an error.
        else if(tokenTable[pointer].kind != procedure)
            error(15);
        // We add the Call command via the emit function.
        emit(op_cal, 0, level - tokenTable[pointer].level, tokenTable[pointer].addr);

        getNextToken(currentNode);
    }
    // If our token is a begin symbol.
    else if (currentToken == beginsym){
        // We call our Statement Procedure again.
        getNextToken(currentNode);
        Statement(currentNode);
        // While our token is a semicolon we get our next token and call statement again.
        while(currentToken == semicolonsym){
            getNextToken(currentNode);
            Statement(currentNode);
        }
        // If our token is not an end symbol we throw an error.
        if(currentToken != endsym)
            error(17);

        getNextToken(currentNode);
    }
    // If our token is an if symbol.
    else if(currentToken == ifsym){
        // We get our next token and call the condition procedure.
        getNextToken(currentNode);
        Condition(currentNode);
        // If our token is not the then symbol we throw an error.
        if(currentToken != thensym)
            error(16);
        // We then get our next node and store our codeIndex into a temp variable. We emit the JPC command and decrement
        // our current register to simulate the freeing up of a register.
        getNextToken(currentNode);
        temp = codeIndex;
        emit(op_jpc, currentRegister, 0, 0);
        currentRegister--;

        Statement(currentNode);
		// If our token is an else symbol.
		if(currentToken == elsesym){
			// We get our next token and hold our index in our codeTemp variable and emit the jmp op code
            getNextToken(currentNode);
            codeTemp = codeIndex;
            emit(op_jmp, 0, 0,0);
			// Our offset at temp gets the code line
            code[temp].m = codeIndex;
			// We call our statement function.
            Statement(currentNode);
			// Our code line is stored in codeTemp as the offset.
            code[codeTemp].m = codeIndex;
        }
        // Else the area we are currently at is marked with the codeIndex so we know where to return from our jump.
		else
            code[temp].m = codeIndex;
	}
    // If our token is an while symbol.
    else if(currentToken == whilesym){
        // Our pointerTemp holds our position in the code.
        pointerTemp = codeIndex;
        // We get the next token in the list and call the procedure Condition. Once we find out our Condition we store
        // the position in the code in codeTemp.
        getNextToken(currentNode);
        Condition(currentNode);
        codeTemp = codeIndex;
        // We then emit the command of JPC.
        emit(op_jpc, currentRegister, 0, 0);
        // If our token is not the do symbol throw an error.
        if(currentToken != dosym)
            error(18);

        getNextToken(currentNode);
        Statement(currentNode);
        // We emit the Jump command l with the addressing being held by pointerTemp.
        emit(op_jmp, 0, 0, pointerTemp);
        // We determine our offset by the codeIndex.
        code[codeTemp].m = codeIndex;
    }
	// If our token is a read symbol.
	else if(currentToken == readsym){
        getNextToken(currentNode);
		// Check to see if our token is the identsym if it isn't throw an error.
        if(currentToken != identsym)
            error(11);
		// Get our next token and set pointer to the value of where our token is in the table.
        getNextToken(currentNode);
        pointer = findToken(currentToken);
		// If the kind isn't a variable throw an error.
        if(tokenTable[pointer].kind != variable)
            error(28);
		// Increment the register.
        currentRegister++;
        // Generate code to read to the current register
        emit(op_sior, currentRegister, 0, 2);
        // Store the value that was read
        emit(op_sto, currentRegister, level-tokenTable[pointer].level, tokenTable[pointer].addr);
		// Decrement the Register.
        currentRegister--;
		// Get the next token.
        getNextToken(currentNode);

    }
	// If our token is a write symbol.
	else if(currentToken == writesym){
        getNextToken(currentNode);
		// check to see if the token is an identsym else throw an error.
        if(currentToken != identsym)
            error(11);
		// Get our next token and set pointer to the value of where our token is in the table.
        getNextToken(currentNode);
        pointer = findToken(currentToken);
		// If the kind isn't a variable throw an error.
        if(tokenTable[pointer].kind != variable)
            error(28);
		// Increment the register.
        currentRegister++;
        // Load the current register
        emit(op_lod, currentRegister, level-tokenTable[pointer].level, tokenTable[pointer].addr);
        // Create code to print to screen
        emit(op_siop, currentRegister, 0, 1);
		// Decrement the register.
        currentRegister--;
		// Get the next token.
        getNextToken(currentNode);
    }
}
// Our Condition procedure.
void Condition(node *currentNode){
    int op;
    // If our token is a odd symbol we then get the next token and call the Expression procedure. Once that is done we
    // emit the Odd command.
    if(currentToken == oddsym){
        getNextToken(currentNode);
        Expression(currentNode);
        emit(op_odd, currentRegister, 0, 0);
    }
    // If its any other condition we call the Expression procedure. Once that is done we then give our op integer the
    // return value from the Relation procedure.
    else{
        Expression(currentNode);
        // Call our Relation procedure and give the return value to our op integer.
        op = rel_op();
        // If op equals 0 then we throw an error.
        if(op == 0)
            error(20);
        // Get our next token and call the Expression procedure.
        getNextToken(currentNode);
        Expression(currentNode);
        // Once were done we then print out the given command depending on what the op code is. We also free up the
        // register by subtracting 1.
        emit(op, currentRegister - 1, currentRegister - 1, currentRegister);
        currentRegister--;
    }
}
// The Relation Procedure
int rel_op(){
    // Switch case that determines what our token is by its given symbol and then returns that op code value.
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
// The Expression procedure.
void Expression(node *currentNode){
    int op;
    // If the token is either a plus or minus symbol we get the next token and call the Term procedure. Once that is
    // complete we then give the NEG command.
    if(currentToken == plussym || currentToken == minussym){
        getNextToken(currentNode);
        Term(currentNode);
        emit(op_neg, currentRegister, currentRegister, 0);
    }
    // If it is not a plus or minus symbol we just call Term.
    else
        Term(currentNode);
    // Continues looping so long as the token is a plus or minus symbol.
    while(currentToken == plussym || currentToken == minussym){
        // The op is given the token and we then get the next token. Then we call the Term procedure.
        op = currentToken;
        getNextToken(currentNode);
        Term(currentNode);
        // If the op is equal to the plus symbol we then emit the ADD command and subtract one from the register.
        if(op == plussym){
            emit(op_add, currentRegister - 1, currentRegister - 1, currentRegister);
            currentRegister--;
        }
        // If the op is equal to the minus symbol we then emit the SUB command and subtract one from the register.
        else if(op == minussym){
            emit(op_sub, currentRegister - 1, currentRegister - 1, currentRegister);
            currentRegister--;
        }
    }
}
// The Term procedure.
void Term(node *currentNode){
    int op;
    // Calls the Factor procedure.
    Factor(currentNode);
    // Continues looping so long as our token is either a mult symbol or slash symbol.
    while(currentToken == multsym || currentToken == slashsym){
        // we then give the op the token value, and get the next token then call the Factor procedure.
        op = currentToken;
        getNextToken(currentNode);
        Factor(currentNode);
        // If the op is equal to the mult symbol we then emit the MUL command and subtract one from the register.
        if(op == multsym){
            emit(op_mul, currentRegister - 1, currentRegister - 1, currentRegister);
            currentRegister--;
        }
        // If the op is equal to the slash symbol we then emit the DIV command and subtract one from the register.
        else if(op == slashsym){
            emit(op_div, currentRegister - 1, currentRegister - 1, currentRegister);
            currentRegister--;
        }
    }
}
// The Factor procedure.
void Factor(node *currentNode){
    int pointer, value;
    // If it is a identifier symbol.
    if(currentToken == identsym){
        // Get the next token, store its position in pointer and add to the register.
        getNextToken(currentNode);
        pointer = findToken(currentToken);
        currentRegister++;
        // If the tokenTable kind is a variable we emit the LOD command.
        if(tokenTable[pointer].kind == variable)
            emit(op_lod, currentRegister, level - tokenTable[pointer].level, tokenTable[pointer].addr);
        // If the tokenTable kind is a constant we emit the LIT command.
        else if(tokenTable[pointer].kind == constant)
            emit(op_lit, currentRegister, 0, tokenTable[pointer].val);
        else if(tokenTable[pointer].kind == variable)
            error(21);
        // If its anything else we throw an error.
        else
            error(26);

        getNextToken(currentNode);
    }
    // If it is a number symbol.
    else if(currentToken == numbersym){
        // We get the next token and then find the value for the given token in the token list. We then add one to the
        // register and emit the LIT command and finally get the next token.
        getNextToken(currentNode);
        value = atoi(tokens[currentToken].name);
        currentRegister++;
        emit(op_lit, currentRegister, 0, value);
        getNextToken(currentNode);
    }
    // If it is a left parenthesis symbol.
    else if(currentToken == lparentsym){
        getNextToken(currentNode);
        Expression(currentNode);
        // If our token is not equal to the right parenthesis symbol we throw an error.
        if(currentToken != rparentsym)
            error(22);

        getNextToken(currentNode);
    }
    // If it is neither of the three possibilities then we throw an error.
    else
        error(23);
}
// Our emit function.
void emit(int op, int r, int l, int m){
    // If we exceed the maximum code length throw an error.
    if(codeIndex > MAX_CODE_LENGTH)
        error(25);
    // We add the given op, r, l, m into the codeIndex of the code array. This stores the generated code and will be
    // used by the VM.
    else{
        code[codeIndex].op = op;
        code[codeIndex].r = r;
        code[codeIndex].l = l;
        code[codeIndex].m = m;
        codeIndex++;
    }
}
// List of our possible errors.
void error(int error){
    printf("Error: ");
    switch(error){

        case 1:
            printf("1. Used = instead of :=.");
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
            printf("6. Incorrect symbol after procedure declaration.");
            break;

        case 7:
            printf("7. Statement expected.");
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
            printf("11. Undeclared identifier.");
            break;

        case 12:
            printf("12. Assignment to constant or procedure is not allowed.");
            break;

        case 13:
            printf("13. Assignment operator.");
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

        case 26:
            printf("26. Unknown variable or constant found.");
            break;

        case 27:
            printf("27. Used := instead of =.");
            break;

        case 28:
            printf("28. Invalid identifier for read or write, must use a variable.");
            break;

        default:
            printf("Unknown error");
            break;
    }
	// Sets our global variable to be true (1) so that we don't print out
	//  a statement saying we have no errors in our function.
	errorFlag = 1;
    printf("\n");
}
// Creates all our nodes. When called it creates a single node and returns it.
node* createNode(int data){
    node *current = (node*)malloc(sizeof(node));

    current->token = data;
    current->next = NULL;

    return current;
}
// Inserts our nodes into the LinkedList. This is done by adding to the end of the tail and returning that node.
node* insertNode(node *head, node *tail, int token){
    if(head == NULL)
        return createNode(token);

    else{
        tail->next = createNode(token);
        return tail->next;
    }
}
// Function that gets our Lexical List we made in the Lexer code.
node *getLexList(){
    int buffer, i;
    char temp [MAX_IDENT_LENGTH + 1];
    FILE *fp;
    node *head, *tail;
    // Opens the text file for reading.
    fp = fopen("lexList.txt", "r");
    // Simple file checking.
    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }
    // We set both the head and tail too null.
    head = tail = NULL;
    // We then get our first token.
    fscanf(fp, " %d", &buffer);
    // We set both the tail and head to be the first node and insert it into our LinkedList.
    head = tail = insertNode(head, tail, buffer);
    // While we are not at the end of the file keep looping and adding to the tail. We also give the tails token to be
    // the buffer.
    while(fscanf(fp, "%d", &buffer) != EOF){
        tail = insertNode(head, tail, buffer);
        tail->token = buffer;
        // Check if the token is going to be a variable.
        if(buffer == 2){
            // Scan the variable name.
            fscanf(fp, "%s", temp);
            // Loop through checking if we've already added the variable to the temp array.
            for(i = 0; i < varCounter; i++){
                // In case we fill up our table and are still looking for if i is in the array. If it reaches above the
                // max size and still has not been placed then we know that it could go in, but do to limitations
                // we will not allow it to go in.
                if(i > MAX_token_TABLE_SIZE)
                    error(25);
                // If we have already added the variable to the list then we just add the variable to the tail
                // and break out of the loop.
                if (strcmp(tokens[i].name, temp) == 0) {
                    tail = insertNode(head, tail, i);
                    tail->token = i;
                    break;
                }
                // If we still have not found the variable by the time we've searched the entire array. Then we must
                // have found an unknown variable and we throw an error. Should never happen, but good to be safe.
                if (i == varCounter - 1){
                    error(26);
                }
            }
        }
        else if(buffer == 3){
            // Scan the constant.
            fscanf(fp, "%s", temp);
            // Loop through checking if we've already added the constant to the temp array.
            for(i = 0; i < varCounter; i++){
                // In case we fill up our table and are still looking for if i is in the array. If it reaches above the
                // max size and still has not been placed then we know that it could go in, but do to limitations
                // we will not allow it to go in.
                if(i > MAX_token_TABLE_SIZE)
                    error(25);
                // If we have already added the constant to the list then we just add the constant to the tail
                // and break out of the loop.
                if (strcmp(tokens[i].name, temp) == 0) {
                    tail = insertNode(head, tail, i);
                    tail->token = i;
                    break;
                }
                // If we still have not found the constant by the time we've searched the entire array. Then we must
                // have found an unknown constant and we throw an error. Should never happen, but good to be safe.
                if (i == varCounter - 1) {
                    error(26);
                }
            }
        }
    }

    fclose(fp);
    // return the head of the LinkedList.
    return head;
}
// Function to fill the symbol table.
void getTokenList(token *tokenList){
    char buffer[MAX_IDENT_LENGTH + 1];
    FILE *fp;
    // Set where we want our counter to be at.
    varCounter = 0;
    //Opens our text file to read.
    fp = fopen("tokenList.txt", "r");
    // Standard error checking.
    if(fp == NULL){
        printf("Error File doesn't exist.");
        exit(1);
    }
    // While were not at the end of the file we copy the buffer into the tokenList and increment count.
    while(fscanf(fp, "%s", buffer) != EOF){
        strcpy(tokenList[varCounter].name, buffer);
        varCounter++;
    }
    // Gives us our final amount of variables we have in our tokenList.
    varCounter++;
    fclose(fp);
}
// Gets the next token in the LinkedList.
void getNextToken(node *currentNode){
    // The current token is the given nodes token.
    currentToken = currentNode->token;
    // So long as we are not at the end of the LinkedList we move the node forward.
    if(currentNode->next != NULL)
        *currentNode = *currentNode->next;

    tokenNum++;
}
// Function that adds the tokens to the tokenTable.
void addTokenTable(int kind, int tokenIndex){
    tokenTableIndex++;
    // Copies the token into the tokenTable.
    strcpy(tokenTable[tokenTableIndex].name, tokens[tokenIndex].name);
    // Gives the tokenTable its level and kind.
    tokenTable[tokenTableIndex].level = level;
    tokenTable[tokenTableIndex].kind = kind;
}
// Finds our token.
int findToken(int token){
    int i;
    // Loops through our tokenTable and compares if they are the same. If they are we return the value of i. If we exit
    // the loop without returning we then return i.
    for(i = tokenTableIndex; i > 0; i--)
        if(strcmp(tokenTable[i].name, tokens[token].name) == 0)
            return i;

    return i;
}
// Function that sends all our code into a text file.
void textForVM(){
    int i = 0;
    FILE *fp;
    // Opens a file called CodeGenOutput and writes to it.
    fp = fopen("CodeGenOutput.txt", "w");
    // We loop through the code array and print out its op, r, l, and m values for every i.
    for(i = 0; i < codeIndex; i++)
        fprintf(fp, "%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);

    fclose(fp);
}
