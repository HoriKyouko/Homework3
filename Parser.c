#include <stdio.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 500

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym
} token_type;

typedef struct  {
	int kind; 		// symbol = 0, const = 1, var = 2, proc = 3, keyword = 4;
	int type;       // token_type
	char name[MAX_IDENT_LENGTH];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
} symbol;

void Program(symbol* tokens, int * currentElement);
void Block(symbol* tokens, int * currentElement, int level);
void ConstDecl(symbol*tokens, int * currentElement);
void VarDecl(symbol*tokens, int*currentElement);
void ProcDecl(symbol*tokens, int*currentElement, int level);
void Statement(symbol*tokens, int*currentElement);
void Condition(symbol * tokens, int * currentElement);
void Expression(symbol*tokens, int *currentElement);
void Term(symbol * tokens, int *currentElement);
void Factor(symbol*tokens, int*currentElement);

int isRelation(int type);

int main(){
    symbol tokens[MAX_SYMBOL_TABLE_SIZE]




}

void Program(symbol* tokens, int * currentElement){
    Block(tokens, currentElement, 0);
    if(tokens[*currentElement].type != periodsym){
        //ERROR
    }
}

void Block(symbol* tokens, int * currentElement, int level){
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

void ConstDecl(symbol*tokens, int * currentElement){

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

void VarDecl(symbol*tokens, int*currentElement){
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

void ProcDecl(symbol*tokens, int*currentElement, int level){
    while(tokens[*currentElement].type == procsym){
        *currentElement = *currentElement+1;
        if(tokens[currentElement].type!= identsym){
            // ERROR
        }
        // ENTER(procedure, ident);
        *currentElement = *currentElement+1;
        if(symbol[*currentElement].type != semicolonsym){
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

void Statement(symbol*tokens, int*currentElement){
    if(tokens[*currentElement].type == identsym){
        *currentElement = *currentElement+1;
        if(tokens[*currentElement].type!= becomessym){
            // Error
        }
        *currentElement = *currentElement+1;
        Expression(symbol,currentElement);
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

void Condition(symbol * tokens, int * currentElement){
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

void Expression(symbol*tokens, int *currentElement){
    if(symbol[*currentElement].type == plussym){
        *currentElement = *currentElement+1;
    }
    Term(tokens, currentElement);
    while(tokens[*currentElement].type == plussym){
        *currentElement = *currentElement+1;
        Term(tokens, currentElement)
    }
}

void Term(symbol * tokens, int *currentElement){
    Factor(tokens, currentElement);
    *currentElement = *currentElement+1;
    while(tokens[*currentElement].type == multsym){
        *currentElement = *currentElement+1;
        Factor(tokens, currentElement)
    }
}

void Factor(symbol*tokens, int*currentElement){
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

int isRelation(int type){
    if(type == eqsym || type == lessym || type == leqsym || type == gtrsym || type == geqsym || type = neqsym){
        return 1;
    }
    return 0;
}

