#include "intercode.h"
#include "semantic.h"
#include "HashMap.h"
#include "Node.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern HashNode HashMap[HASHMAP_SIZE];

CodeList InterCodes;
int variable_num = 0, label_num = 0;

char* trans_InterCode(InterCode code){
    char* output = malloc(sizeof(char) * 100);
    memset(output, 0, sizeof(char) * 100);
    if(code -> kind == LABEL_i){
        char* x = trans_Operand(code -> u.op);
        sprintf(output, "LABEL %s :", x);
    }
    return output;
}

char* trans_Operand(Operand op){
    char* tmp = malloc(sizeof(char) * 100);
    if(op -> kind == CONSTANT) sprintf(tmp, "#%d", op -> u.val);
    else if(op -> kind == LABEL) sprintf(tmp, "l%d", op -> u.label_id);
    else sprintf(tmp, "t%d", op -> u.variable_id);
    char* output = malloc(sizeof(char) * 101);
    strcpy(output, tmp);
    return output;
}

void Add_intercode(CodeList code){ // Add Code to the tail of  [intercodes_head -> intercodes_tail]
    if (code == NULL) return;
    while(InterCodes != NULL) InterCodes = InterCodes -> next;
    InterCodes = code;
}

CodeList Join_intercode(CodeList code1, CodeList code2){
    CodeList tmp = code1;
    while(tmp != NULL) tmp = tmp -> next;
    tmp = code2;
    return code1;
}


CodeList trans_FunDec(struct Node* node){
    /*
    FunDec → ID LP VarList RP
    | ID LP RP
    */
    struct Node* child = node -> child;
    FieldList look = lookup_hash(child -> TYPE_ID);
    assert(look != NULL && look -> type -> kind == FUNCTION);
    FieldList paras = look -> type -> u.func.argv;
    InterCode tmp = new_intercode(FUNC_i);
    tmp -> u.func = child -> TYPE_ID;
    CodeList code = new_codelist(tmp);
    while(paras != NULL){
        InterCode para = new_intercode(PARAM_i);
        Operand op = NULL;
        if (paras -> type -> kind == BASIC) op = new_temp(VARIABLE);
        else if (paras -> type -> kind == ARRAY /*|| paras -> type -> kind == STRUCTURE*/) op = new_temp(ADDRESS);
        para -> u.op = op;
        FieldList look = lookup_hash(paras -> name); // param has a global scope
        look -> op = op;
        code = Join_intercode(code, new_codelist(para));
        paras = paras -> tail;
    }
}

CodeList trans_Stmt(struct Node* node){
    /*
    Stmt → Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    struct Node* child = node -> child;
    if (!strcmp(child -> name, "CompSt\0") && childsize(node) == 1) return trans_CompSt(child);
    if (!strcmp(child -> name, "Exp\0") && childsize(node) == 2) return trans_Exp(child, NULL);
    if (!strcmp(child -> name, "RETURN\0") && childsize(node) == 3){
        Operand tmp = new_temp(VARIABLE);
        CodeList code1 = trans_Exp(child -> brother, tmp);
        InterCode code = new_intercode(RET_i);
        code -> u.op = tmp;
        CodeList code2 = new_codelist(code);
        return Join_intercode(code1, code2);
    }
    if (!strcmp(child -> name, "IF\0") && childsize(node) == 5){
        Operand tmp1 = new_label(), tmp2 = new_label();
        CodeList code1 = trans_Cond(child -> brother -> brother, tmp1, tmp2);
        CodeList code2 = trans_Stmt(child -> brother -> brother -> brother -> brother);
        InterCode l1 = new_intercode(LABEL_i), l2 = new_intercode(LABEL_i);
        l1 -> u.op = tmp1, l2 -> u.op = tmp2;
        CodeList L1 = new_codelist(l1), L2 = new_codelist(l2);
        return Join_intercode(Join_intercode(code1, L1), Join_intercode(code2, L2));
    }
}

CodeList trans_StmtList(struct Node* node){
    /*
    StmtList → Stmt StmtList
    | e
    */
    CodeList code = NULL;
    struct Node* tmp = node -> child;
    while(tmp != NULL){
        code = Join_intercode(code, trans_Stmt(tmp));
        tmp = tmp -> brother;
    }
    return code;
}

CodeList trans_CompSt(struct Node* node){
    /*
    CompSt → LC DefList StmtList RC
    */
    struct Node* child = node -> child -> brother;
    CodeList code1 = trans_DefList(child);
    CodeList code2 = trans_StmtList(child -> brother);
    return Join_intercode(code1, code2);
}

void trans_ExtDef(struct Node* node){
    /*
    ExtDef → Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */
    struct Node* child = node -> child -> brother;
    if (!strcmp(child -> name, "FunDec\0") && childsize(node) == 3){
        struct Node* brother = child -> brother;
        Join_intercode(trans_FunDec(child), trans_CompSt(brother));
    }
}   

void trans_ExtDefList(struct Node* node){
    /*
    ExtDefList → ExtDef ExtDefList
    | e
    */
    struct Node* tmp = node -> child;
    while(tmp != NULL){
        trans_ExtDef(tmp);
        tmp = tmp -> brother;
    }
}

void trans_Program(struct Node* node){
    /*
    Program → ExtDefList
    */
    struct Node* child = node -> child;
    if (child != NULL) trans_ExtDefList(child);
}

void build_inter_code(struct Node* root){ 
    if (root == NULL) return;
    //if (!strcmp(root -> name, "ExtDef")) Add_intercode(trans_ExtDef(root));
    //struct Node* child = node -> child;
    //for (; child != NULL; child = child -> brother) build_inter_code(child);

}

void inter_code(char* file, struct Node* root){
    //build_inter_code(root);
    trans_Program(root);
    output(file);
}

Operand new_label(){
    Operand tmp = (Operand) malloc(sizeof(Operand));
    tmp -> kind = LABEL;
    tmp -> u.label_id = label_num;
    label_num++;
    return tmp;
}

Operand new_temp(int kind){
    Operand tmp = (Operand) malloc(sizeof(Operand));
    tmp -> kind = kind;
    tmp -> u.variable_id = variable_num;
    variable_num++;
    return tmp;
}

Operand new_constant(int val){
    Operand tmp = (Operand) malloc(sizeof(Operand));
    tmp -> kind = CONSTANT;
    tmp -> u.val = val;
    return tmp;
}

InterCode new_intercode(int kind){
    InterCode tmp = (InterCode) malloc(sizeof(InterCode));
    tmp -> kind = kind;
    return tmp;
}

CodeList new_codelist(InterCode code){
    CodeList tmp = (CodeList) malloc(sizeof(CodeList));
    tmp -> code = code;
    tmp -> next = NULL;
    return tmp;
}

Operand look_up_hash(struct Node* node){
    char* name = node -> TYPE_ID;
    FieldList tmp = lookup_hash(name);
    assert(tmp != NULL);
    if (tmp -> op == NULL) tmp -> op = new_temp(VARIABLE);
    return tmp -> op;
}

int childsize(struct Node* father){
    int size = 0;
    struct Node* child = father -> child;
    if (child == NULL) return 0;
    while(child != NULL){
        size++;
        child = child -> brother;
    }
    return size;
}

void output(char* file){ // print three address code into file
    FILE* fp = fopen(file, "w+");
    if (file == NULL) { printf("Empty file!\n"); perror(file); return; }
    if (!fp) { printf("No avaiable file!\n"); perror(file); return; }

    CodeList tmp = InterCodes;
    while(tmp != NULL){
        char* line = trans_InterCode(tmp -> code);
        if (strlen(line)) fprintf(fp, "%s\n", line);
        tmp = tmp -> next;
    }
    fclose(fp);
}


