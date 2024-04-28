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
    enum{LABEL_i, FUNC_i, ASSIGN_i, PLUS_i, MINUS_i, MUL_i, DIV_i, GOTO_i, IF_GOTO_i, RET_i, DEC_i, ARG_i, CALL_i, PARAM_i, CHANGE_ADDR, READ, WRITE} kind;
    union{
        Operand op;
        char* func;
        struct { Operand left, right; } assign;
        struct { Operand operand1, operand2, result; } binop;
    }u;
};


struct CodeList_{
    InterCode code;
    CodeList next; 
};

char* trans_InterCode(InterCode code);

char* trans_Operand(Operand op);

void Add_intercode(CodeList code);

CodeList Join_intercode(CodeList code1, CodeList code2);

CodeList trans_FunDec(struct Node* node);

CodeList trans_Stmt(struct Node* Stmt);

CodeList trans_Exp(struct Node* node, Operand place);

CodeList trans_DefList(struct Node* DefList);

CodeList trans_StmtList(struct Node* StmtList);

CodeList trans_CompSt(struct Node* node);

void trans_ExtDecList(struct Node* node);

void trans_ExtDef(struct Node* node);

void trans_ExtDefList(struct Node* node);

void trans_Program(struct Node* node);

void build_inter_code(struct Node* root);

void inter_code(char* file, struct Node* root);

Operand new_label();

Operand new_temp();

Operand new_constant();

InterCode new_intercode(int kind);

CodeList new_codelist(InterCode code);

Operand look_up_hash(struct Node* node);

int childsize(struct Node* father);

void output(char* file);

#endif