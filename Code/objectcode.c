#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "objectcode.h"
#include "intercode.h"
struct Register_ regs[REGS_NUM];
char* reg_name[]={"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
Var_List var_list;
int stack_offset;
unsigned para_num;
int arg_num;
void translate(char*file){
    FILE* fp;
    if (file == NULL) { printf("Empty file!\n"); }
    else {
        fp = fopen(file, "w+");
        if (!fp) { printf("No avaiable file!\n"); perror(file); return; }
    }
    init(fp);
    trans_codes(fp,InterCodes);
    fclose(fp);
}
void init(FILE *fp)
{
    for(int i=0;i<REGS_NUM;i++)
    {
        regs[i].name=reg_name[i];
        regs[i].state=FREE;
        regs[i].var=NULL;
    }
    var_list=NULL;
    fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, ".globl main\n");
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n");
    fprintf(fp, "\n");

    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0, 1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0, $0\n");
    fprintf(fp, "  jr $ra\n");
    fprintf(fp, "\n");
}
void trans_codes(FILE *fp,CodeList codelist){
    CodeList tmp=codelist;
    int i=0;
    while (tmp!=NULL)
    {
        trans_one_code(fp,tmp);
        tmp=tmp->next;
    }
    
}
void trans_one_code(FILE*fp, CodeList onecode){
    InterCode code=onecode->code;
    switch (code->kind)
    {
        case LABEL_i:{
            fprintf(fp,"label%d:\n",code->u.op->u.label_id);
            break;
        }
        case FUNC_i:{
            var_list=NULL;
            para_num=0;
            stack_offset=0;
            arg_num=0;
            fprintf(fp,"%s:\n",code->u.func);
            fprintf(fp,"  move $fp, $sp\n");
            //printf("stack_offset:%d\n",stack_offset);
            fprintf(fp,"  addi $sp, $sp, %d\n",cal_offset(onecode));
            //printf("stack_offset:%d\n",stack_offset);
            break;
        }
        case ASSIGN_i:{
            Operand left=code->u.assign.left;
            Operand right=code->u.assign.right;
            Variable var_left=(Variable)get_var(left);
            if(var_left==NULL)
            {   if(left->kind!=CONSTANT)
                {
                    //printf("ASSIGN var_left is NULL\n");
                    //printf("left->num:%d\n",left->u.variable_id);
                }
                var_left=(Variable)insert_var(left);
            }
            unsigned lefti=get_reg(fp,left,1);
            unsigned righti=get_reg(fp,right,0);
            Variable var_right=(Variable)get_var(right);
            if(var_right==NULL)
               {    if(right->kind!=CONSTANT)
                     //printf("ASSIGN var_right is NULL\n"); 
                var_right=(Variable)insert_var(right);
               }
            fprintf(fp,"  move %s, %s\n",regs[lefti].name,regs[righti].name);
            save_reg(fp,lefti);
            free_reg(righti);
            break;
        }
        case PLUS_i:{
            Operand result=code->u.binop.result;
            Operand operand1=code->u.binop.operand1;
            Operand operand2=code->u.binop.operand2;
            Variable var_result=(Variable)get_var(result);
            if(var_result==NULL){
                if(result->kind!=CONSTANT)
                //printf("PLUS var_result is NULL\n");
                var_result=(Variable)insert_var(result);
            }
            unsigned resulti=get_reg(fp,result,1);
            unsigned operand1i=get_reg(fp,operand1,0);
            unsigned operand2i=get_reg(fp,operand2,0);
            fprintf(fp,"  add %s, %s, %s\n",regs[resulti].name,regs[operand1i].name,regs[operand2i].name);
            save_reg(fp,resulti);
            free_reg(operand1i);
            free_reg(operand2i);
            break;
        }
        case MINUS_i:{
            Operand result=code->u.binop.result;
            Operand operand1=code->u.binop.operand1;
            Operand operand2=code->u.binop.operand2;
            Variable var_result=(Variable)get_var(result);
            if(var_result==NULL)
               var_result=(Variable)insert_var(result);
            //printf("MINUS var_result is NULL\n");}
            unsigned resulti=get_reg(fp,result,1);
            unsigned operand1i=get_reg(fp,operand1,0);
            unsigned operand2i=get_reg(fp,operand2,0);
            fprintf(fp,"  sub %s, %s, %s\n",regs[resulti].name,regs[operand1i].name,regs[operand2i].name);
            save_reg(fp,resulti);
            free_reg(operand1i);
            free_reg(operand2i);
            break;
        }
        case MUL_i:{
            Operand result=code->u.binop.result;
            Operand operand1=code->u.binop.operand1;
            Operand operand2=code->u.binop.operand2;
            Variable var_result=(Variable)get_var(result);
            if(var_result==NULL)
            {
                var_result=(Variable)insert_var(result);
                //printf("MUL var_result is NULL\n");
            }
            unsigned resulti=get_reg(fp,result,1);
            unsigned operand1i=get_reg(fp,operand1,0);
            unsigned operand2i=get_reg(fp,operand2,0);
            fprintf(fp,"  mul %s, %s, %s\n",regs[resulti].name,regs[operand1i].name,regs[operand2i].name);
            save_reg(fp,resulti);
            free_reg(operand1i);
            free_reg(operand2i);
            break;
        }
        case DIV_i:{    
            Operand result=code->u.binop.result;
            Operand operand1=code->u.binop.operand1;
            Operand operand2=code->u.binop.operand2;
            Variable var_result=(Variable)get_var(result);
            if(var_result==NULL){
                //printf("DIV var_result is NULL\n");
                var_result=(Variable)insert_var(result);}
            unsigned resulti=get_reg(fp,result,1);
            unsigned operand1i=get_reg(fp,operand1,0);
            unsigned operand2i=get_reg(fp,operand2,0);
            fprintf(fp,"  div %s, %s\n",regs[operand1i].name,regs[operand2i].name);
            fprintf(fp,"  mflo %s\n",regs[resulti].name);
            save_reg(fp,resulti);
            free_reg(operand1i);
            free_reg(operand2i);
            break;
        }
        case GOTO_i:{
            fprintf(fp,"  j label%d\n",code->u.op->u.label_id);
            break;
        }
        case IF_GOTO_i:{
            Operand x=code->u.if_goto.x;
            Operand y=code->u.if_goto.y;
            Operand z=code->u.if_goto.z;
            char* relop=code->u.if_goto.relop;
            unsigned xi=get_reg(fp,x,0);
            unsigned yi=get_reg(fp,y,0);
            unsigned zi=get_reg(fp,z,0);
            if(!strcmp(relop,"==")){
                fprintf(fp,"  beq %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            else if(!strcmp(relop,"!=")){
                fprintf(fp,"  bne %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            else if(!strcmp(relop,">")){
                fprintf(fp,"  bgt %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            else if(!strcmp(relop,"<")){
                fprintf(fp,"  blt %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            else if(!strcmp(relop,">=")){
                fprintf(fp,"  bge %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            else if(!strcmp(relop,"<=")){
                fprintf(fp,"  ble %s, %s, label%d\n",regs[xi].name,regs[yi].name,z->u.label_id);
            }
            free_reg(xi);
            free_reg(yi);
            free_reg(zi);
            break;
        }
        case RET_i:{
            Operand op=code->u.op;
            unsigned opi=get_reg(fp,op,0);    
            fprintf(fp,"  move $v0, %s\n",regs[opi].name);
            fprintf(fp,"  jr $ra\n");
            free_reg(opi);
            //printf("stack_offset:%d\n",stack_offset);
            break;
            
        }
        case DEC_i:{
            Operand x=code->u.dec.x;
            int size=code->u.dec.size;
            stack_offset-=size-4;
            struct VarNode* var=get_var(x);
            if(var==NULL)
            {
                //printf("DEC var is NULL\n");
                var=insert_var(x);
            } 
            break;
            
        }
        case ARG_i:{
            Operand op=code->u.op;
            unsigned opi=get_reg(fp,op,0);
            fprintf(fp,"  addi $sp, $sp, -4\n");
            fprintf(fp,"  sw %s, 0($sp)\n",regs[opi].name);
            arg_num++;
            free_reg(opi);
            break;
            
        }
        case CALL_i:{
            char* func=code->u.call.func;
            fprintf(fp,"  addi $sp, $sp, -8\n");
            fprintf(fp,"  sw $ra, 4($sp)\n");
            fprintf(fp,"  sw $fp, 0($sp)\n");
            fprintf(fp,"  move $fp, $sp\n");
            fprintf(fp,"  jal %s\n",func);
            fprintf(fp,"  move $sp, $fp\n");
            fprintf(fp,"  lw $fp, 0($sp)\n");
            fprintf(fp,"  lw $ra, 4($sp)\n");
            fprintf(fp,"  addi $sp, $sp, 8\n");
            fprintf(fp,"  addi $sp, $sp, %d\n",arg_num*4);
            arg_num=0;
            Operand result=code->u.call.result;
            if(get_var(result)==NULL)
            {
                    //printf("CALL var is NULL\n");
                    insert_var(result);
            }
            unsigned resulti=get_reg(fp,result,1);
            fprintf(fp,"  move %s, $v0\n",regs[resulti].name);
            save_reg(fp,resulti);
            break;
        }
        case PARAM_i:{
            break;
        }
        case CHANGE_ADDR:{
            Operand left=code->u.assign.left;
            Operand right=code->u.assign.right;
            Variable var_left=(Variable)get_var(left);
            unsigned lefti=get_reg(fp,left,1);
            unsigned righti=get_reg(fp,right,0);
            Variable var_right=(Variable)get_var(right);
            if(var_right==NULL){
                //printf("CHANGE_ADDR var_right is NULL\n");
                var_right=(Variable)insert_var(right);}
            fprintf(fp,"  move %s, %s\n",regs[lefti].name,regs[righti].name);
            save_reg(fp,lefti);
            free_reg(righti);
            break;
        }
        case READ:{
            fprintf(fp,"  addi $sp, $sp, -4\n");
            fprintf(fp,"  sw $ra, 0($sp)\n");
            fprintf(fp,"  jal read\n");
            fprintf(fp,"  lw $ra, 0($sp)\n");
            fprintf(fp,"  addi $sp, $sp, 4\n");
            Operand result=code->u.op;
            if(get_var(result)==NULL)
            {
                    //printf("READ var is NULL\n");
                    insert_var(result);
            }
            unsigned resulti=get_reg(fp,result,1);
            fprintf(fp,"  move %s, $v0\n",regs[resulti].name);
            save_reg(fp,resulti);
            break;
        }
        case WRITE:{
            unsigned resulti=get_reg(fp,code->u.op,0);
            fprintf(fp,"  move $a0, %s\n",regs[resulti].name);
            fprintf(fp,"  addi $sp, $sp, -4\n");
            fprintf(fp,"  sw $ra, 0($sp)\n");
            fprintf(fp,"  jal write\n");
            fprintf(fp,"  lw $ra, 0($sp)\n");
            fprintf(fp,"  addi $sp, $sp, 4\n");
            free_reg(resulti);
            break;
        }
        default:{
            printf("error\n");
            break;
        }
            
        
    }
}   

unsigned get_reg(FILE *fp, Operand op,int left){
    unsigned int i;
    for(i=8;i<16;i++)
    {
        if(regs[i].state==FREE)
        {
            regs[i].state=BUSY;
            break;
        }
    }
    if(op->kind==CONSTANT){
        fprintf(fp,"  li %s, %d\n",regs[i].name,op->u.val);
    }
    else if(op->kind==VARIABLE){
        regs[i].var=get_var(op)->var;
        struct VarNode* var=get_var(op);
        if(!left)
        fprintf(fp,"  lw %s, %d($fp)\n",regs[i].name,var->var->offset);
        int offset=var->var->offset;
    }
    return i;
}
struct VarNode* get_var(Operand op){
    Var_List tmp=var_list;
    while (tmp!=NULL)
    {
        if(tmp->var->op==op)
            return tmp;
        tmp=tmp->next;
    }
    return NULL;
}
struct VarNode* insert_var(Operand op){
    if(op->kind==CONSTANT)
        return NULL;
    Var_List tmp=var_list;
    Var_List new_var=(Var_List)malloc(sizeof(struct VarNode));
    new_var->var=(Variable)malloc(sizeof(struct Variable_));
    stack_offset-=4;
    new_var->var->op=op;
    new_var->var->offset=stack_offset;
    new_var->var->reg_num=-1;
    new_var->next=NULL;
    if(tmp==NULL)
    {
        var_list=new_var;
        return new_var;
    }
    while (tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    tmp->next=new_var;
    
    return new_var;
}
void free_reg(unsigned reg_num){
    regs[reg_num].state=FREE;
    regs[reg_num].var=NULL;
}
void save_reg(FILE *fp,unsigned reg_num){
    Variable var=regs[reg_num].var;
    if(var==NULL)
        return;
    else{
        fprintf(fp,"  sw %s, %d($fp)\n",regs[reg_num].name,var->offset);
    }
    free_reg(reg_num);
}
int cal_offset(CodeList codelist){

    CodeList tmp=codelist->next;
    while (tmp!=NULL&&tmp->code->kind!=FUNC_i)
    {   InterCode code=tmp->code;
        if(tmp->code->kind==DEC_i)
        {
            Operand x=code->u.dec.x;
            int size=code->u.dec.size;
            stack_offset-=size-4;
            struct VarNode* var=get_var(x);
            if(var==NULL)
                var=insert_var(x);
        }
        else if(tmp->code->kind==ASSIGN_i)
        {
            Operand left=code->u.assign.left;
            Operand right=code->u.assign.right;
            Variable var_left=(Variable)get_var(left);
            if(var_left==NULL)
            {
                var_left=(Variable)insert_var(left);
            }
            Variable var_right=(Variable)get_var(right);
            if(var_right==NULL)
                var_right=(Variable)insert_var(right);
            
        }
        else if(tmp->code->kind==PLUS_i||tmp->code->kind==MINUS_i||tmp->code->kind==MUL_i||tmp->code->kind==DIV_i)
        {
            Operand result=code->u.binop.result;
            Variable var_result=(Variable)get_var(result);
            if(var_result==NULL)
                var_result=(Variable)insert_var(result);
        }
        else if (tmp->code->kind==CALL_i)
        {
            Operand result=code->u.call.result;
            if(get_var(result)==NULL)
            {
                insert_var(result);
            }
            
        }
        else if(tmp->code->kind==CHANGE_ADDR){
            Operand left=code->u.assign.left;
            Operand right=code->u.assign.right;
            Variable var_left=(Variable)get_var(left);
            Variable var_right=(Variable)get_var(right);
            if(var_left==NULL)
                var_left=(Variable)insert_var(right); 
        }
        else if(tmp->code->kind==READ){
            Operand result=code->u.call.result;
            if(get_var(result)==NULL)
            {
                insert_var(result);
            }
        }
        else if(tmp->code->kind==WRITE){
            Operand result=code->u.call.result;
            if(get_var(result)==NULL)
            {
                insert_var(result);
            }
        }
        else if(tmp->code->kind==PARAM_i){
            Operand op=code->u.op;
            Var_List tmp=var_list;
            Var_List new_var=(Var_List)malloc(sizeof(struct VarNode));
            new_var->var=(Variable)malloc(sizeof(struct Variable_));
            new_var->var->op=op;
            new_var->var->offset=8+4*para_num;
            new_var->var->reg_num=-1;
            new_var->next=NULL;
            if(tmp==NULL)
            {
                var_list=new_var;
            }
            else{
                while (tmp->next!=NULL)
                {
                    tmp=tmp->next;
                }
                tmp->next=new_var;
            }
            para_num++;
        }
        
        tmp=tmp->next;
    }
    return stack_offset;
}