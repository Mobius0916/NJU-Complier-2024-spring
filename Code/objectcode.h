#ifndef OBJECTCODE
#define OBJECTCODE
#include "HashMap.h"
#include "intercode.h"
#include <stdio.h>
#define REGS_NUM 32
typedef struct Register_* Register; 
typedef struct Variable_* Variable ;
typedef struct VarNode * Var_List;
struct Register_{
    char* name;
    enum {BUSY, FREE}state;
    Variable var;
};

extern struct Register_ regs[REGS_NUM];

struct Variable_
{
    Operand op;
    unsigned offset;
    unsigned reg_num;  
};

struct VarNode
{
    Variable var;
    struct VarNode * next;
};

extern Var_List var_list;

void translate(char *file);
void init(FILE *fp);
void trans_codes(FILE *fp,CodeList codelist);
void trans_one_code(FILE* fp,CodeList onecode);
unsigned get_reg(FILE *fp,Operand op,int left);
struct VarNode* get_var(Operand op);
struct VarNode* insert_var(Operand op);
void free_reg(unsigned reg_num);
void save_reg(FILE *fp,unsigned reg_num);
int cal_offset(CodeList codelist);
#endif