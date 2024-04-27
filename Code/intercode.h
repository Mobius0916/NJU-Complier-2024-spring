#ifndef INTERCODE
#define INTERCODE
#include "Node.h"
#include "HashMap.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct CodeList_* CodeList;

struct Operand_{
    enum {VARIABLE, ADDRESS, LABEL, CONSTANT} kind;
    union{
        int variable_id;
        int label_id;
        int val;
    }u;
};

struct InterCode_{
    enum{LABEL_i, FUNC_i, ASSIGN_i, PLUS_i, MINUS_i, MUL_i, DIV_i, GOTO_i, IF_GOTO_i, RET_i, DEC_i, ARG_i, CALL_i, PARAM_i, READ, WRITE} kind;
    union{
        Operand op;

    }u;
};


struct CodeList_{
    InterCode code;
    CodeList next; 
};

char* trans_InterCode(InterCode code);

char* trans_Operand(Operand op);

void inter_code(char* file, struct Node* root);

void build_inter_code(struct Node* root);

void Add_intercode(CodeList code);

void Join_intercode(CodeList code1, CodeList code2);

Operand new_label();

Operand new_temp();

Operand new_constant();

Operand look_up_hash(struct Node* node);

void output(char* file);

#endif