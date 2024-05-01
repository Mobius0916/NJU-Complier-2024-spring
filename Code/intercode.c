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
int variable_num = 0, label_num = 1;

void output(char* file){ // print three address code into file
    FILE* fp;
    if (file == NULL) { printf("Empty file!\n"); }
    else {
        fp = fopen(file, "w+");
        if (!fp) { printf("No avaiable file!\n"); perror(file); return; }
    }

    CodeList tmp = InterCodes;
    while(tmp != NULL){
        char* line = trans_InterCode(tmp -> code);
        if (strlen(line) && file != NULL) fprintf(fp, "%s\n", line);
        else if (strlen(line)) printf("%s\n", line);
        tmp = tmp -> next;
    }
    if (file != NULL) fclose(fp);
}

char* trans_InterCode(InterCode code){ // trans intercode into three address code
    char* output = malloc(sizeof(char) * 100);
    memset(output, 0, sizeof(char) * 100);
    if(code -> kind == LABEL_i){
        char* x = trans_Operand(code -> u.op);
        sprintf(output, "LABEL %s :", x);
    }
    else if (code -> kind == FUNC_i){
        char* func_name = code -> u.func;
        sprintf(output, "FUNCTION %s :", func_name);
    }
    else if (code -> kind == ASSIGN_i){
        Operand left = code -> u.assign.left, right = code -> u.assign.right;
        if (code -> u.assign.left != NULL){
            char* l = trans_Operand(left);
            char* r = trans_Operand(right);
            if (left -> kind == ADDRESS && right -> kind != ADDRESS) sprintf(output, "%s := &%s", l, r);
            else if (left -> kind != ADDRESS && right -> kind == ADDRESS) sprintf(output, "%s := *%s", l, r);
            else sprintf(output, "%s := %s", l, r);
        }
    }
    else if (code -> kind == CHANGE_ADDR){
        if (code -> u.assign.left != NULL){
            char* l = trans_Operand(code -> u.assign.left);
            char* r = trans_Operand(code -> u.assign.right);
            sprintf(output, "*%s := %s", l, r);
        }
    }
    else if (code -> kind == PLUS_i || code -> kind == MINUS_i || code -> kind == MUL_i || code -> kind == DIV_i){
        if (code -> u.binop.result != NULL){
            char* result = trans_Operand(code -> u.binop.result);
            char* op1 = trans_Operand(code -> u.binop.operand1);
            char* op2 = trans_Operand(code -> u.binop.operand2);
            if (code -> kind == PLUS_i) sprintf(output, "%s := %s + %s", result, op1, op2);
            else if (code -> kind == MINUS_i) sprintf(output, "%s := %s - %s", result, op1, op2);
            else if (code -> kind == MUL_i) sprintf(output, "%s := %s * %s", result, op1, op2);
            else if (code -> kind == DIV_i) sprintf(output, "%s := %s / %s", result, op1, op2);
        }
    }
    else if (code -> kind == GOTO_i){
        char* x = trans_Operand(code -> u.op);
        sprintf(output, "GOTO %s", x);
    }
    else if (code -> kind == IF_GOTO_i){
        char* x = trans_Operand(code -> u.if_goto.x);
        char* y = trans_Operand(code -> u.if_goto.y);
        char* z = trans_Operand(code -> u.if_goto.z);
        char* relop = code -> u.if_goto.relop;
        sprintf(output, "IF %s %s %s GOTO %s", x, relop, y, z);
    }
    else if (code -> kind == RET_i){
        char* x = trans_Operand(code -> u. op);
        sprintf(output, "RETURN %s", x);
    }
    else if (code -> kind == DEC_i){
        char* x = trans_Operand(code -> u.dec.x);
        int size = code -> u.dec.size;
        sprintf(output, "DEC %s %d", x, size);
    }
    else if (code -> kind == ARG_i){
        char* x = trans_Operand(code -> u.op);
        if (code -> u.op -> kind == ADDRESS) sprintf(output, "ARG %s", x);   
        sprintf(output, "ARG %s", x);
    }
    else if (code -> kind == CALL_i){
        char* result = trans_Operand(code -> u.call.result);
        char* func_name = code -> u.call.func;
        sprintf(output, "%s := CALL %s", result, func_name);
    }
    else if (code -> kind == PARAM_i){
        char* x = trans_Operand(code -> u.op);
        sprintf(output, "PARAM %s", x);
    }
    else if (code -> kind == READ){
        char *x = trans_Operand(code -> u.op);
        sprintf(output, "READ %s", x);
    }
    else if (code -> kind == WRITE){
        char *x = trans_Operand(code -> u.op);
        sprintf(output, "WRITE %s", x);
    }
    return output;
}

char* trans_Operand(Operand op){ 
    assert(op != NULL);
    char* tmp = malloc(sizeof(char) * 100);
    if(op -> kind == CONSTANT) sprintf(tmp, "#%d", op -> u.val);
    else if(op -> kind == LABEL) sprintf(tmp, "label%d", op -> u.label_id);
    else sprintf(tmp, "t%d", op -> u.variable_id);
    char* output = malloc(sizeof(char) * 101);
    strcpy(output, tmp);
    return output;
}

CodeList Join_intercode(CodeList code1, CodeList code2){ // Link code2 to the end of code1 and return the linked list 
    if (code1 == NULL) return code2; 
    CodeList tmp = code1;
    while (tmp -> next != NULL) tmp = tmp -> next;
    tmp -> next = code2; 
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
    return code;
}

CodeList trans_Stmt(struct Node* Stmt){
    /*
    Stmt → Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    struct Node* child = Stmt -> child;
    if (!strcmp(child -> name, "CompSt\0") && childsize(Stmt) == 1) return trans_CompSt(child);
    if (!strcmp(child -> name, "Exp\0") && childsize(Stmt) == 2) return trans_Exp(child, NULL); 
    if (!strcmp(child -> name, "RETURN\0") && childsize(Stmt) == 3){
        Operand tmp = new_temp(VARIABLE);
        CodeList code1 = trans_Exp(child -> brother, tmp);
        InterCode code = new_intercode(RET_i);
        code -> u.op = tmp;
        CodeList code2 = new_codelist(code);
        return Join_intercode(code1, code2);
    }
    if (!strcmp(child -> name, "IF\0") && childsize(Stmt) == 5){
        Operand tmp1 = new_label(), tmp2 = new_label();
        CodeList code1 = trans_Cond(child -> brother -> brother, tmp1, tmp2);
        CodeList code2 = trans_Stmt(child -> brother -> brother -> brother -> brother);
        InterCode l1 = new_intercode(LABEL_i), l2 = new_intercode(LABEL_i);
        l1 -> u.op = tmp1, l2 -> u.op = tmp2;
        CodeList L1 = new_codelist(l1), L2 = new_codelist(l2);
        return Join_intercode(Join_intercode(code1, L1), Join_intercode(code2, L2));
    }
    if (!strcmp(child -> name, "IF\0") && childsize(Stmt) == 7){
        Operand tmp1 = new_label(), tmp2 = new_label(), tmp3 = new_label();
        CodeList code1 = trans_Cond(child -> brother -> brother, tmp1, tmp2);
        CodeList code2 = trans_Stmt(child -> brother -> brother -> brother -> brother);
        CodeList code3 = trans_Stmt(child -> brother -> brother -> brother -> brother -> brother -> brother);
        InterCode l1  = new_intercode(LABEL_i), l2 = new_intercode(LABEL_i), l3_goto = new_intercode(GOTO_i), l3_label = new_intercode(LABEL_i);
        l1 -> u.op = tmp1, l2 -> u.op = tmp2, l3_goto -> u.op = tmp3, l3_label -> u.op = tmp3;
        CodeList L1 = new_codelist(l1), L2 = new_codelist(l2), L3_goto = new_codelist(l3_goto), L3_label = new_codelist(l3_label);
        return Join_intercode(Join_intercode(Join_intercode(code1, L1), Join_intercode(Join_intercode(code2, L3_goto), L2)), Join_intercode(code3, L3_label));
    }
    if (!strcmp(child -> name, "WHILE\0") && childsize(Stmt) == 5){
        Operand tmp1 = new_label(), tmp2 = new_label(), tmp3 = new_label();
        CodeList code1 = trans_Cond(child -> brother -> brother, tmp2, tmp3);
        CodeList code2 = trans_Stmt(child -> brother -> brother -> brother -> brother);
        InterCode l1_label = new_intercode(LABEL_i), l1_goto = new_intercode(GOTO_i), l2 = new_intercode(LABEL_i), l3 = new_intercode(LABEL_i);
        l1_label -> u.op = tmp1, l1_goto -> u.op = tmp1, l2 -> u.op = tmp2, l3 -> u.op = tmp3;
        CodeList L1_label = new_codelist(l1_label), L1_goto = new_codelist(l1_goto), L2 = new_codelist(l2), L3 = new_codelist(l3);
        return Join_intercode(Join_intercode(Join_intercode(L1_label, code1), Join_intercode(L2, code2)), Join_intercode(L1_goto, L3));
    }
}

CodeList trans_Args(struct Node* Args, FieldList* arg_list){
    /*
    Args → Exp COMMA Args
    | Exp
    */
    Operand tmp1 = new_temp(VARIABLE);
    if (Exp(Args -> child) -> kind != BASIC) tmp1 -> kind = ADDRESS;
    CodeList code1 = trans_Exp(Args -> child, tmp1);
    FieldList tmp = (FieldList) malloc(sizeof(struct FieldList_));
    tmp -> op = tmp1;
    tmp -> tail = *arg_list;
    *arg_list = tmp;
    if (Args -> child -> brother == NULL) return code1;
    else{
        CodeList code2 = trans_Args(Args -> child -> brother -> brother, arg_list);
        return Join_intercode(code1, code2);
    }
}

CodeList trans_Exp(struct Node* node, Operand place){
    /*
    Exp → y Exp ASSIGNOP Exp
    x | Exp AND Exp
    x | Exp OR Exp
    x | Exp RELOP Exp
    y | Exp PLUS Exp
    y | Exp MINUS Exp
    y | Exp STAR Exp
    y | Exp DIV Exp
    y | LP Exp RP
    y | MINUS Exp
    x | NOT Exp
    y | ID LP Args RP
    y | ID LP RP
    y | Exp LB Exp RB
    y | Exp DOT ID
    y | ID
    y | INT
    y | FLOAT
    */
    if (childsize(node) == 1){
        if (place == NULL) return NULL;
        if (!strcmp(node -> child -> name, "INT\0")){
            InterCode code = new_intercode(ASSIGN_i);
            code -> u.assign.left = place;
            code -> u.assign.right = new_constant(node -> child -> TYPE_INT);
            CodeList code1 = new_codelist(code);
            return code1;
        }
        if (!strcmp(node -> child -> name, "FLOAT\0")){
            InterCode code = new_intercode(ASSIGN_i);
            code -> u.assign.left = place;
            code -> u.assign.right = new_constant(node -> child -> TYPE_FLOAT);
            CodeList code1 = new_codelist(code);
            return code1;
        }
        if (!strcmp(node -> child -> name, "ID\0")){
            char* name = node -> child -> TYPE_ID;
            FieldList tmp = lookup_hash(name);
            Operand op = look_up_hash(node -> child);
            InterCode code = new_intercode(ASSIGN_i);
            code -> u.assign.left = place;
            code -> u.assign.right = op;
            CodeList code1 = new_codelist(code);
            return code1;
        }
    }
    
    if (childsize(node) == 2){
        if (!strcmp(node -> child -> name, "MINUS\0")){
            Operand tmp1 = new_temp(VARIABLE);
            CodeList code1 = trans_Exp(node -> child -> brother, tmp1);
            CodeList code2 = NULL;
            if (place != NULL){
                InterCode code = new_intercode(MINUS_i);
                code -> u.binop.operand1 = new_constant(0);
                code -> u.binop.operand2 = tmp1;
                code -> u.binop.result = place;
                code2 = new_codelist(code);
            }
            return Join_intercode(code1, code2);
        }
    }

    if (childsize(node) == 3){
        if (!strcmp(node -> child -> brother -> name, "ASSIGNOP\0")){
            struct Node* child = node -> child;
            if (childsize(child) == 1 && !strcmp(child -> child -> name, "ID\0")){ //Exp1 is ID
                Operand variable_ = look_up_hash(child -> child);
                Operand tmp1 = new_temp(VARIABLE);
                CodeList code1 = trans_Exp(node -> child -> brother -> brother, tmp1);
                InterCode code = new_intercode(ASSIGN_i);
                code -> u.assign.left = variable_, code -> u.assign.right = tmp1; 
                CodeList code2 = new_codelist(code);
                if (place != NULL){
                    code = new_intercode(ASSIGN_i);
                    code -> u.assign.left = place;
                    code -> u.assign.right = variable_;
                    code2 = Join_intercode(code2, new_codelist(code));
                }
                return Join_intercode(code1, code2);
            }
            else if (childsize(child) > 1){ //Exp1 is array or struct elem
                Operand array_ = new_temp(ADDRESS);
                Operand tmp1 = new_temp(VARIABLE);
                CodeList code0 = trans_Exp(node -> child, array_);
                CodeList code1 = trans_Exp(node -> child -> brother -> brother, tmp1);
                InterCode code = new_intercode(CHANGE_ADDR);
                code -> u.assign.left = array_;
                code -> u.assign.right = tmp1;
                CodeList code2 = new_codelist(code);
                if (place != NULL){
                    code = new_intercode(ASSIGN_i);
                    code -> u.assign.left = place;
                    code -> u.assign.right = tmp1;
                    code2 = Join_intercode(code2, new_codelist(code));
                }
                return Join_intercode(code0, Join_intercode(code1, code2));
            }
  
        }

        else if (!strcmp(node -> child -> brother -> name, "PLUS\0") || !strcmp(node -> child -> brother -> name, "MINUS\0") || !strcmp(node -> child -> brother -> name, "STAR\0") || !strcmp(node -> child -> brother -> name, "DIV\0")){
            Operand tmp1 = new_temp(VARIABLE), tmp2 = new_temp(VARIABLE);
            CodeList code1 = trans_Exp(node -> child, tmp1);
            CodeList code2 = trans_Exp(node -> child -> brother -> brother, tmp2);
            CodeList code3 = NULL;
            if (place != NULL){
                InterCode code = (InterCode) malloc(sizeof(struct InterCode_)); 
                code -> u.binop.operand1 = tmp1;
                code -> u.binop.operand2 = tmp2;
                code -> u.binop.result = place;
                if (!strcmp(node -> child -> brother -> name, "PLUS\0")) code -> kind = PLUS_i;
                else if (!strcmp(node -> child -> brother -> name, "MINUS\0")) code -> kind = MINUS_i;
                else if (!strcmp(node -> child -> brother -> name, "STAR\0")) code -> kind = MUL_i;
                else if (!strcmp(node -> child -> brother -> name, "DIV\0")) code -> kind = DIV_i;
                code3 = new_codelist(code);
            }
            return Join_intercode(Join_intercode(code1, code2), code3);
        }

        else if (!strcmp(node -> child -> name, "LP\0")) return trans_Exp(node -> child -> brother, place);

        else if (!strcmp(node -> child -> brother -> name, "LP\0")){
            FieldList look = lookup_hash(node -> child -> TYPE_ID);
            if (!strcmp(look -> name, "read\0")){
                if (place == NULL) return NULL;
                InterCode code = new_intercode(READ);
                code -> u.op = place;
                return new_codelist(code);
            }
            else{
                InterCode code = new_intercode(CALL_i);
                if (place != NULL) code -> u.call.result = place; 
                else code -> u.call.result = new_temp(VARIABLE);
                code -> u.call.func = look -> name;
                return new_codelist(code);
            }
        }

        else if (!strcmp(node -> child -> brother -> name, "DOT\0")){
            if (place == NULL) return NULL;
            Type type = Exp(node -> child);
            Operand tmp1 = new_temp(ADDRESS);
            CodeList code1 = trans_Exp(node -> child, tmp1);
            int offset = get_struct_offset(type, node -> child -> brother -> brother -> TYPE_ID);
            InterCode code = new_intercode(PLUS_i);
            code -> u.binop.operand1 = tmp1;
            code -> u.binop.operand2 = new_constant(offset);
            code -> u.binop.result = place;
            CodeList code2 = new_codelist(code);
            if (place -> kind == VARIABLE){
                Operand tmp2 = new_temp(ADDRESS);
                code -> u.binop.result = tmp2;
                code = new_intercode(ASSIGN_i);
                code -> u.assign.left = place;
                code -> u.assign.right = tmp2;
                code2 = Join_intercode(code2, new_codelist(code));
            }
            return Join_intercode(code1, code2);
        }
    }
    if (childsize(node) == 4){
        if (!strcmp(node -> child -> brother -> name, "LB\0")){ // EXP1 LB EXP2 RB   
            Operand tmp1 = new_temp(ADDRESS), tmp2 = new_temp(VARIABLE), tmp3 = new_temp(VARIABLE);
            CodeList code1 = trans_Exp(node -> child, tmp1); // EXP1
            CodeList code2 = trans_Exp(node -> child -> brother -> brother, tmp2); // EXP2
            InterCode code = new_intercode(MUL_i);
            code -> u.binop.operand1 = tmp2;
            code -> u.binop.operand2 = new_constant(get_array_size(node));
            code -> u.binop.result = tmp3;
            CodeList code3 = new_codelist(code);
            CodeList code4 = NULL;
            if (place != NULL){
                code = new_intercode(PLUS_i);
                code -> u.binop.operand1 = tmp1;
                code -> u.binop.operand2 = tmp3;
                code -> u.binop.result = place;
                code4 = new_codelist(code);
                if (place -> kind == VARIABLE){
                    Operand tmp4 = new_temp(ADDRESS);
                    code -> u.binop.result = tmp4;
                    code = new_intercode(ASSIGN_i);
                    code -> u.assign.left = place;
                    code -> u.assign.right = tmp4;
                    code4 = Join_intercode(code4, new_codelist(code));
                }
            }
            return Join_intercode(Join_intercode(code1, code2), Join_intercode(code3, code4));
        }

        else if (!strcmp(node -> child -> brother -> brother -> name, "Args\0")){
            FieldList look = lookup_hash(node -> child -> TYPE_ID);
            FieldList arg_list = NULL;
            CodeList code1 = trans_Args(node -> child -> brother -> brother, &arg_list);
            if (!strcmp(look -> name, "write\0")){
                InterCode code = new_intercode(WRITE);
                code -> u.op = arg_list -> op;
                CodeList code2 = new_codelist(code);
                CodeList code3 = NULL;
                if (place != NULL){
                    code = new_intercode(ASSIGN_i);
                    code -> u.assign.left = place;
                    code -> u.assign.right = new_constant(0);
                    code3 = new_codelist(code);
                } 
                return Join_intercode(code1, Join_intercode(code2, code3));
            }
            else{
                CodeList code2 = NULL;
                while(arg_list != NULL){
                    InterCode code = new_intercode(ARG_i);
                    code -> u.op = arg_list -> op;
                    code2 = Join_intercode(code2, new_codelist(code));
                    arg_list = arg_list -> tail;
                }
                InterCode code = new_intercode(CALL_i);
                if (place != NULL) code -> u.call.result = place;
                else code -> u.call.result = new_temp(VARIABLE);
                code -> u.call.func = look -> name;
                CodeList code3 = new_codelist(code);
                return Join_intercode(code1, Join_intercode(code2, code3));
            }
        }
    }
}

CodeList trans_Cond(struct Node* node, Operand label_true, Operand label_false){
    /*
    Exp → x Exp ASSIGNOP Exp
    y  | Exp AND Exp
    y  | Exp OR Exp
      | Exp RELOP Exp
    x | Exp PLUS Exp
    x | Exp MINUS Exp
    x | Exp STAR Exp
    x | Exp DIV Exp
    x | LP Exp RP
    x | MINUS Exp
    y | NOT Exp
    x | ID LP Args RP
    x | ID LP RP
    x | Exp LB Exp RB
    x | Exp DOT ID
    x | ID
    x | INT
    x | FLOAT
    */
    if (childsize(node) == 2) return trans_Cond(node -> child -> brother, label_false, label_true);
    if (childsize(node) == 3){
        if (!strcmp(node -> child -> brother -> name, "AND\0")){
            Operand label1 = new_label();
            CodeList code1 = trans_Cond(node -> child, label1, label_false);
            CodeList code2 = trans_Cond(node -> child -> brother -> brother, label_true, label_false);
            InterCode code = new_intercode(LABEL_i);
            code -> u.op = label1;
            CodeList codeLABEL = new_codelist(code);
            return Join_intercode(code1, Join_intercode(codeLABEL, code2));
        }
        else if (!strcmp(node -> child -> brother -> name, "OR\0")){
            Operand label1 = new_label();
            CodeList code1 = trans_Cond(node -> child, label_true, label1);
            CodeList code2 = trans_Cond(node -> child -> brother -> brother, label_true, label_false);
            InterCode code = new_intercode(LABEL_i);
            code -> u.op = label1;
            CodeList codeLABEL = new_codelist(code);
            return Join_intercode(code1, Join_intercode(codeLABEL, code2));
        }
        else if (!strcmp(node -> child -> brother -> name, "RELOP\0")){
            Operand tmp1 = new_temp(VARIABLE);
            Operand tmp2 = new_temp(VARIABLE);
            CodeList code1 = trans_Exp(node -> child, tmp1);
            CodeList code2 = trans_Exp(node -> child -> brother -> brother, tmp2);
            char* relop = node -> child -> brother -> RELOP_;
            InterCode code = new_intercode(IF_GOTO_i);
            code -> u.if_goto.relop = relop;
            code -> u.if_goto.x = tmp1;
            code -> u.if_goto.y = tmp2;
            code -> u.if_goto.z = label_true;
            CodeList code3 = new_codelist(code);
            code = new_intercode(GOTO_i);
            code -> u.op = label_false;
            CodeList code4 = new_codelist(code);
            return Join_intercode(Join_intercode(code1, code2), Join_intercode(code3, code4));
        }
    }
    Operand tmp1 = new_temp(VARIABLE);
    CodeList code1 = trans_Exp(node, tmp1);
    InterCode code = new_intercode(IF_GOTO_i);
    strcpy(code -> u.if_goto.relop, "!=");
    code -> u.if_goto.x = tmp1;
    code -> u.if_goto.y = new_constant(0);
    code -> u.if_goto.z = label_true;
    CodeList code2 = new_codelist(code);
    code = new_intercode(GOTO_i);
    code -> u.op = label_false;
    CodeList code3 = new_codelist(code);
    return Join_intercode(code1, Join_intercode(code2, code3));
}

CodeList trans_Dec(struct Node* Dec){
    /* ju bu bian liang
    Dec → VarDec
    | VarDec ASSIGNOP Exp
    */
    assert(Dec != NULL && Dec -> child != NULL);
    struct Node* VarDec = Dec -> child;
    if (VarDec -> brother != NULL && !strcmp(VarDec -> brother -> name, "ASSIGNOP\0")){
        if (!strcmp(VarDec -> child -> name, "ID\0")){
            Operand op = look_up_hash(VarDec -> child);
            return trans_Exp(Dec -> child -> brother -> brother, op);
        }
    }
    else{ //DEC   array or structure  (9)
        struct Node* ID = get_VarDec_ID(VarDec);
        FieldList look = lookup_hash(ID -> TYPE_ID);
        if (look -> type -> kind == ARRAY || look -> type -> kind == STRUCTURE){
            Operand op = new_temp(ARR_STRU);
            InterCode code = new_intercode(DEC_i);
            int size = getsize(look -> type);
            code -> u.dec.size = size;
            code -> u.dec.x = op;
            look -> op = op;
            return new_codelist(code);
        } 
        else return NULL;
    }
}

CodeList trans_DecList(struct Node* DecList){
    /*
    DecList → Dec
    | Dec COMMA DecList
    */
    CodeList code = NULL;
    struct Node* tmp = DecList;
    while(tmp != NULL){
        struct Node* Dec = tmp -> child;
        if (Dec != NULL) code = Join_intercode(code, trans_Dec(Dec));
        if (Dec -> brother == NULL) return code;
        else tmp = tmp -> child -> brother -> brother;
    }
    return code;
}

CodeList trans_Def(struct Node* Def){
    /*
    Def → Specifier DecList SEMI
    */
    assert(Def -> child != NULL || Def -> child -> brother != NULL);
    struct Node* DecList = Def -> child -> brother;
    return trans_DecList(DecList);
}

CodeList trans_DefList(struct Node* DefList){
    /*
    DefList → Def DefList
    | e
    */
    CodeList code = NULL;
    while(DefList != NULL){
        struct Node* Def = DefList -> child;
        if (Def != NULL){
            code = Join_intercode(code, trans_Def(Def));
            DefList = Def -> brother;
        }
        else break;
    }
    return code;
}

CodeList trans_StmtList(struct Node* StmtList){
    /*
    StmtList → Stmt StmtList
    | e
    */
    CodeList code = NULL;
    while (StmtList != NULL){
        struct Node* Stmt = StmtList -> child;
        if (Stmt != NULL) {
            code = Join_intercode(code, trans_Stmt(Stmt));
            StmtList = Stmt -> brother;
        }
        else break;
    }
    return code;
}

CodeList trans_CompSt(struct Node* CompSt){
    /*
    CompSt → LC DefList StmtList RC
    */
    assert(CompSt -> child != NULL);
    struct Node* child = CompSt -> child -> brother;
    CodeList code1 = trans_DefList(child);
    CodeList code2 = trans_StmtList(child -> brother);
    return Join_intercode(code1, code2);
}

CodeList trans_ExtDef(struct Node* ExtDef){
    /*
    ExtDef → Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */
    assert(ExtDef -> child != NULL);
    struct Node* child = ExtDef -> child -> brother;
    if (childsize(ExtDef) == 3 && !strcmp(child -> name, "FunDec\0")){
        struct Node* brother = child -> brother;
        CodeList code1 = trans_FunDec(child);
        CodeList code2 = trans_CompSt(brother);
        return Join_intercode(code1, code2);
    }
    return NULL;
}   

CodeList trans_ExtDefList(struct Node* ExtDefList){
    /*
    ExtDefList → ExtDef ExtDefList
    | e
    */
    CodeList code = NULL;
    while (ExtDefList != NULL){
        struct Node* ExtDef = ExtDefList -> child;
        if (ExtDefList -> child != NULL) {
            code = Join_intercode(code, trans_ExtDef(ExtDef));
            ExtDefList = ExtDefList -> child -> brother;
        }
        else break;
    }
    return code;
}

CodeList trans_Program(struct Node* Program){
    /*
    Program → ExtDefList
    */
    struct Node* ExtDefList = Program -> child;
    if (ExtDefList != NULL) return trans_ExtDefList(ExtDefList);
    else return NULL; 
}

void inter_code(char* file, struct Node* root){
    InterCodes = trans_Program(root);
    output(file);
}

Operand new_label(){
    Operand tmp = (Operand) malloc(sizeof(struct Operand_));
    tmp -> kind = LABEL;
    tmp -> u.label_id = label_num;
    label_num++;
    return tmp;
}

Operand new_temp(int kind){
    Operand tmp = (Operand) malloc(sizeof(struct Operand_));
    tmp -> kind = kind;
    tmp -> u.variable_id = variable_num;
    variable_num++;
    return tmp;
}

Operand new_constant(int val){
    Operand tmp = (Operand) malloc(sizeof(struct Operand_));
    tmp -> kind = CONSTANT;
    tmp -> u.val = val;
    return tmp;
}

InterCode new_intercode(int kind){
    InterCode tmp = (InterCode) malloc(sizeof(struct InterCode_));
    tmp -> kind = kind;
    return tmp;
}

CodeList new_codelist(InterCode code){
    CodeList tmp = (CodeList) malloc(sizeof(struct CodeList_));
    tmp -> code = code;
    tmp -> next = NULL;
    return tmp;
}

Operand look_up_hash(struct Node* node){
    assert(node -> TYPE_ID != NULL);
    char* name = node -> TYPE_ID;
    FieldList tmp = lookup_hash(name);
    assert(tmp != NULL);
    if (tmp -> op == NULL) tmp -> op = new_temp(VARIABLE);
    return tmp -> op;
}

int childsize(struct Node* father){
    int size = 0;
    struct Node* child = father -> child;
    while(child != NULL){
        size++;
        child = child -> brother;
    }
    return size;
}

struct Node* get_VarDec_ID(struct Node* VarDec){
    struct Node* tmp = VarDec;
    while(strcmp(tmp -> name, "ID\0") != 0) tmp = tmp -> child;
    return tmp;
}



