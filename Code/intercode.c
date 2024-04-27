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

void Join_intercode(CodeList code1, CodeList code2){
    CodeList tmp = code1;
    while(tmp != NULL) tmp = tmp -> next;
    tmp = code2;
}


void build_inter_code(struct Node* node){ 
    if (root == NULL) return;
    //if (!strcmp(root -> name, "ExtDef")) Add_intercode(trans_ExtDef(root));
    struct Node* child = node -> child;
    for (; child != NULL; child = child -> brother) build_inter_code(child);
}

void inter_code(char* file, struct Node* root){
    build_inter_code(root);
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

Operand look_up_hash(struct Node* node){
    char* name = node -> TYPE_ID;
    FieldList tmp = lookup_hash(name);
    assert(tmp != NULL);
    if (tmp -> op == NULL) tmp -> op = new_temp(VARIABLE);
    return tmp -> op;
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


