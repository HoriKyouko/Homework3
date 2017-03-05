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
    if(tokens[*currentElement].type != periodsym){
        //ERROR
    }
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
            //  ERROR
        }
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= eqsym){
            //  ERROR
        }
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= numsym){
            //  ERROR
        }
        *currentElement = *currentElement+1;

    } while(currentToken.type == commasym);
    if(tokens[*currentElement].type != semicolonsym){
        // ERROR
    }
    *currentElement = *currentElement+1;

}

void VarDecl(token*tokens, int*currentElement){
    do{
        *currentElement = *currentElement + 1;
        if(tokens[*currentElement].type!=identsym){
            //  ERROR
        }
        *currentElement = *currentElement+1;

    } while(currentToken.type == commasym);
    if(tokens[*currentElement].type != semicolonsym){
        // ERROR
    }
    *currentElement = *currentElement+1;
}

void ProcDecl(token*tokens, int*currentElement, int level){
    while(tokens[*currentElement].type == procsym){
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= identsym){
            // ERROR
        }
        // ENTER(procedure, ident);
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type != semicolonsym){
            // ERROR
        }
        *currentElement = *currentElement+1;
        Block(tokens, currentElement, level+1);
        if(tokens[*currentElement].type!=semicolonsym){
            // ERROR
        }
        *currentElement = *currentElement+1;
    }
}

void Statement(token*tokens, int*currentElement){
    if(tokens[*currentElement].type == identsym){
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= becomessym){
            // Error
        }
        *currentElement = *currentElement+1;
        Expression(tokens,currentElement);
    } else if(tokens[*currentElement].type==callsym){
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!=identsym){
            // Error
        }
        *currentElement = *currentElement+1;
    } else if (tokens[*currentElement].type == beginsym]){
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement)
        while(tokens[*currentElement].type == semicolonsym){
            *currentElement = *currentElement+1;
            Statement(tokens,currentElement);
        }
        if(tokens[*currentElement].type!=endsym){
            // ERROR
        }
        *currentElement = *currentElement+1;
    } else if(tokens[*currentElement].type == ifsym){
        *currentElement = *currentElement+1;
        Condition(tokens, currentElement);
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement)
    } else if(sokens[*currentElement].type == whilesym){
        *currentElement = *currentElement+1;
        Condition(tokens, currentElement);
        if(tokens[*currentElement].type!= dosym){
            // ERROR
        }
        *currentElement = *currentElement+1;
        Statement(tokens,currentElement);
    }
}

void Condition(token * tokens, int * currentElement){
    if(tokens[*currentElement].type == oddsym){
        *currentElement = *currentElement+1;
        Expression(tokens,currentElement);
    } else{
        Expression(tokens, currentElement);
        if(!isRelation(tokens[*currentElement].type)){
            // ERROR
        }
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
    } else if(tokens[*currentElement].type == numbersym){
        *currentElement = *currentElement+1;
    } else if(tokens[*currentElement].type == lparentsym){
        *currentElement = *currentElement+1;
        Expression(tokens, currentElement);
        if(tokens[*currentElement].type != rparentsym){
            // ERROR
        }
        *currentElement = *currentElement+1;
    } else{
        // ERROR
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
    if(type == eqsym || type == lessym || type == leqsym || type == gtrsym || type == geqsym || type = neqsym){
        return 1;
    }
    return 0;
}

