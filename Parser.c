#include <stdio.h>
#include <string.h>

#define MAX_token_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500

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
    op_odd, op_mod, op_eql, op_lss, op_leq, op_gtr, op_geq
} op_type

typedef struct  {
	int kind; 		// symbol = 0, const = 1, var = 2, proc = 3, keyword = 4;
	int type;       // token_type
	char name[MAX_IDENT_LENGTH];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
} token;

typedef struct {
int op;		// op code
int r;      // register
int  l;   	// level
int  m;   	// moffset
} instruction;

typedef enum{
	constant = 1,
	variable,
	procedure
}symbolKind;

void Program(token* tokens, int * currentElement);
void Block(token* tokens, int * currentElement, int level);
void ConstDecl(token*tokens, int * currentElement);
void VarDecl(token*tokens, int*currentElement);
void ProcDecl(token*tokens, int*currentElement, int level);
void Statement(token*tokens, int*currentElement);
void Condition(token * tokens, int * currentElement);
void Expression(token*tokens, int *currentElement);
void Term(token * tokens, int *currentElement);
void Factor(token*tokens, int*currentElement);

int isRelation(int type);

int main(){
    int currentElement = 0;
    int cx = 0;

    instruction code[];
    token tokens[MAX_token_TABLE_SIZE]

    Program(tokens, &currentElement)



}

void Program(token* tokens, int * currentElement){
    Block(tokens, currentElement, 0);
    
    if(tokens[*currentElement].type != periodsym)
        error(6);
}

void Block(token* tokens, int * currentElement, int level){
    if(tokens[*currentElement].type == constsym){
        ConstDecl(tokens, currentElement);
    if(tokens[*currentElement.type == varsym){
        VarDecl(tokens,currentElement);
    }
    if(tokens.[*currentElement].type == procsym){
        ProcDecl(tokens,currentElement, level);
    }
    Statement(tokens,currentElement);
}

void ConstDecl(token*tokens, int * currentElement){

    do{
        *currentElement = *currentElement + 1;
        if(tokens[*currentElement].type!=identsym){
            error(4);
        }
        
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= eqsym){
            if(currentElement == becomessym)
                error(1);
            else
                error(3);
        }
        
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= numsym)
            error(2);
            
        *currentElement = *currentElement+1;

    } while(currentToken.type == commasym);
    
    if(tokens[*currentElement].type != semicolonsym)
        error(5);
    
    *currentElement = *currentElement+1;

}

void VarDecl(token*tokens, int*currentElement){
    do{
        *currentElement = *currentElement + 1;
        
        if(tokens[*currentElement].type!=identsym)
            error(4);
        
        *currentElement = *currentElement+1;

    } while(currentToken.type == commasym);
    
    if(tokens[*currentElement].type != semicolonsym)
        error(5);
        
    *currentElement = *currentElement+1;
}

void ProcDecl(token*tokens, int*currentElement, int level){
    while(tokens[*currentElement].type == procsym){
        *currentElement = *currentElement+1;
        
        if(tokens[*currentElement].type!= identsym)
            error(4);
        
        // ENTER(procedure, ident);
        *currentElement = *currentElement+1;
        
        if(tokens[*currentElement].type != semicolonsym)
            error(5);
        
        *currentElement = *currentElement+1;
        Block(tokens, currentElement, level+1);
        
        if(tokens[*currentElement].type!=semicolonsym)
            error(5);
        
        *currentElement = *currentElement+1;
    }
}

void Statement(token*tokens, int*currentElement){
    
    if(tokens[*currentElement].type == identsym){
        *currentElement = *currentElement+1;
        
        if(tokens[*currentElement].type!= becomessym)
            error(9);
            
        *currentElement = *currentElement+1;
        Expression(tokens,currentElement);
    }// end ident
    
    else if(tokens[*currentElement].type==callsym){
        *currentElement = *currentElement+1;
        
        if(tokens[*currentElement].type!=identsym)
            error(23);

        *currentElement = *currentElement+1;
    }// end call
    
    else if (tokens[*currentElement].type == beginsym]){
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement)
        while(tokens[*currentElement].type == semicolonsym){
            *currentElement = *currentElement+1;
            Statement(tokens,currentElement);
        }
        if(tokens[*currentElement].type!=endsym)
            error(11);
        
        *currentElement = *currentElement+1;
    }// end begin
    
    else if(tokens[*currentElement].type == ifsym){
        *currentElement = *currentElement+1;
        Condition(tokens, currentElement);
        
        if(tokens[*currentElement].type != thensym)
            error(10);
        // More stuff goes here
        
        if(tokens[*currentElement].type == elsesym){
            // Stuff goes here as well
        }
            
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement)
    }// end if
    
    else if(tokens[*currentElement].type == whilesym){
        *currentElement = *currentElement+1;
        Condition(tokens, currentElement);
        
        if(tokens[*currentElement].type!= dosym)
            error(12);
        
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement);
    }// end while
    
    else if(tokens[*currentElement].type == readsym){
        *currentElement = *curentElement+1;
        
        if(*currentElement != identsym)
            error(18);
        
        //Some stuff here
        
        //If the symbol table kind doesn't equal variable throw error(11)
        
        // More stuff
    }
    
    else if(tokens[*currentElement].type == writesym){
        *currentElement = *currentElement+1;
        
        if(*currentElement != identsym)
            error(18);
        
        // some stuff here
        
        // If the symbol table kind doesn't equal variable then throw error(11)
        
        //more stuff
    }
}

void Condition(token * tokens, int * currentElement){
    if(tokens[*currentElement].type == oddsym){
        *currentElement = *currentElement+1;
        Expression(tokens,currentElement);
    } 
    
    else{
        Expression(tokens, currentElement);
        
        if(!isRelation(tokens[*currentElement].type))
            error(13);
        
        *currentElement = *currentElement+1;
        Expression(tokens, currentElement);
    }
}

void Expression(token*tokens, int *currentElement){
    int addOp = tokens[*currentElement].type;
    if(tokens[*currentElement].type == plussym || tokens[*currentElement].type == minussym){
        *currentElement = *currentElement+1;
        if(addOp == minussym){
            // need to figure out r, l , and m
            emit(op_neg, 0, 0, 0);
        }
    }
    Term(tokens, currentElement);
    while(tokens[*currentElement].type == plussym || tokens[*currentElement].type == minussym){
        *currentElement = *currentElement+1;
        Term(tokens, currentElement)
    }
}

void Term(token * tokens, int *currentElement){
    Factor(tokens, currentElement);
    *currentElement = *currentElement+1;
    while(tokens[*currentElement].type == multsym || tokens[*currentElement].type == slashsym){
        *currentElement = *currentElement+1;
        Factor(tokens, currentElement)
    }
}

void Factor(token*tokens, int*currentElement){
    
    if(tokens[*currentElement].type == identsym){
        *currentElement = *currentElement+1;
        // Some more code
        // If the symbol table kind doesn't equal variable or constant throw error(14)
    } 
    
    else if(tokens[*currentElement].type == numbersym){
        *currentElement = *currentElement+1;
    }
    
    else if(tokens[*currentElement].type == lparentsym){
        *currentElement = *currentElement+1;
        Expression(tokens, currentElement);
        if(tokens[*currentElement].type != rparentsym){
            error(15);
        }
        *currentElement = *currentElement+1;
    }
    
    else{
        error(16);
    }
}

void emit(int op, int r, int l, int m, int * cx){
    if(*cx > MAX_CODE_LENGTH){
        // ERROR
    } else{
        code[*cx].op = op;
        code[*cx].r = r;
        code[*cx].l = l;
        code[*cx].m = m;
        *cx = *cx+1;
    }

}

int isRelation(int type){
    // SHould be a switch statement I feel.
    if(type == eqsym || type == lessym || type == leqsym || type == gtrsym || type == geqsym || type = neqsym){
        return 1;
    }
    return 0;
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
    defalut:
        printf("Unknown error");
        break;
    }
    printf("\n");
    exit(1);
}
