#ifndef INTERCODE
#define INTERCODE
#include "Node.h"
#include "HashMap.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct FieldList_* FieldList;
typedef struct CodeList_* CodeList;

struct Operand_{
    enum {VARIABLE, ADDRESS, LABEL, CONSTANT, ARR_STRU} kind;
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
        struct { Operand x; int size; } dec;
        struct { Operand result; char* func; } call;
        struct { Operand x, y, z; char* relop; } if_goto;
    }u;
};


struct CodeList_{
    InterCode code;
    CodeList next; 
};

char* trans_InterCode(InterCode code);

char* trans_Operand(Operand op);

CodeList Join_intercode(CodeList code1, CodeList code2);

CodeList trans_FunDec(struct Node* node);

CodeList trans_Stmt(struct Node* Stmt);

CodeList trans_Args(struct Node* Args, FieldList* argv);

CodeList trans_Exp(struct Node* node, Operand place);

CodeList trans_Cond(struct Node* node, Operand label_true, Operand label_false);

CodeList trans_Dec(struct Node* Dec);

CodeList trans_DecList(struct Node* DecList);

CodeList trans_Def(struct Node* Def);

CodeList trans_DefList(struct Node* DefList);

CodeList trans_StmtList(struct Node* StmtList);

CodeList trans_CompSt(struct Node* node);

CodeList trans_ExtDef(struct Node* node);

CodeList trans_ExtDefList(struct Node* node);

CodeList trans_Program(struct Node* node);

void inter_code(char* file, struct Node* root);

Operand new_label();

Operand new_temp();

Operand new_constant();

InterCode new_intercode(int kind);

CodeList new_codelist(InterCode code);

Operand look_up_hash(struct Node* node);

int childsize(struct Node* father);

struct Node* get_VarDec_ID(struct Node* VarDec);

void output(char* file);

#endif