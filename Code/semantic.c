#include"semantic.h"
#include"Node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"HashMap.h"
unsigned SemanticError=0;
void semantic_anyasis(struct Node* node)
{
	init_hash();
	//printf("1");
	Program(node);
	CheckUndefFunc();
}
void Program(struct Node* node){
	struct Node* child = node->child;
	ExtDefList(child);
};

void ExtDefList(struct Node* node){
	struct Node* child = node -> child;
	if(child != NULL){
		ExtDef(child);
		ExtDefList(child -> brother);
	}
};

void ExtDef(struct Node* node){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	Type type = Specifier(child);
	if(!strcmp(brother -> name, "ExtDecList\0")){
		ExtDecList(brother, type);
	}
	else if(!strcmp(brother -> name, "FunDec\0")){
		struct Node* brother1 = brother -> brother;
		if(!strcmp(brother1 -> name, "CompSt\0")){
			FunDec(brother, type, 0);
			if(type != NULL && type -> kind != ERROR)
			CompSt(brother1, type);
		}
		else{
			FunDec(brother, type, node -> lineNum);
		}

	}
};

void ExtDecList(struct Node* node, Type type){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	if(brother == NULL){
		VarDec(child, type, NULL);
	}
	else{
		VarDec(child, type, NULL);
		struct Node* brother2 = brother -> brother;
		ExtDecList(brother2, type);
	}
};

Type Specifier(struct Node* node){
	struct Node* child = node -> child;
	Type type = (Type) malloc(sizeof(struct Type_));
	if(!strcmp(child -> name, "TYPE\0")){
		if(child -> is_type == 1){
			type -> kind = BASIC;
			type -> u.basic = INT;
		}
		if(child -> is_type == 2){
			type -> kind = BASIC;
			type -> u.basic = FLOAT;
		}
	}
	else{
		StructSpecifier(child, type);
	}
	//printf("%d\n", getsize(type));
	return type;
};

void StructSpecifier(struct Node* node, Type type){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	type -> kind = STRUCTURE;
	if(!strcmp(brother -> name, "OptTag\0")){
		struct Node* brother1 = child -> brother;
		struct Node* brother2 = brother1 -> brother -> brother;
		char* opt = OptTag(brother);
		if(lookup_hash(opt) != NULL){
			PrintSemErr(16, node -> lineNum, opt);
			SemanticError++;
			type -> kind = ERROR;
			return;
		}
		FieldList field = (FieldList) malloc(sizeof(struct FieldList_));
		field -> tail = NULL;
		field -> type = (Type) malloc(sizeof(struct Type_));
		field -> type -> kind = STRUCTTAG;
		field -> type -> u.structmember = NULL;
		field -> name = opt;
		DefList(brother2, field);
		insert_hash(field);
		type -> u.structure = field;
	}
	else{
		char* tag = Tag(brother);
		FieldList field = lookup_hash(Tag(brother));
		if(field == NULL){
			PrintSemErr(17, node -> lineNum, tag);
			SemanticError++;
			type -> kind = ERROR;
			return;
		}
		type -> u.structure = field;
	}
};

char* OptTag(struct Node* node)
{
	struct Node* child = node -> child;
	char* name = (char*) malloc(sizeof(char)*32);
	if(child != NULL){
		strcpy(name, child -> TYPE_ID);
		return name;
	}
	else{
		return NULL;
	}
};

char* Tag(struct Node* node)
{
	struct Node* child = node -> child;
	char* name = (char*) malloc(sizeof(char)*32);
	strcpy(name, child -> TYPE_ID);
	return name;
};

FieldList VarDec(struct Node* node, Type type, FieldList field){
	struct Node* child = node -> child;
	FieldList VarField = NULL;
	if(!strcmp(child -> name, "ID\0")){
		VarField = (FieldList) malloc(sizeof(struct FieldList_));
		VarField -> name = (char*) malloc(sizeof(char)*32);
		strcpy(VarField -> name, child -> TYPE_ID);
		VarField -> type = type;
		VarField -> tail = NULL;
		FieldList look = lookup_hash(child -> TYPE_ID);
		if(field == NULL || field -> type -> kind == FUNCTION){
			FieldList look2 = NULL;
			if(field != NULL && field -> name != NULL){	
				look2 = lookup_hash(field -> name);
			}
			if(look != NULL){//compst and varlist
				if((look2 != NULL && (look2 -> type -> kind == FUNCTION && look2 -> type -> u.func.is_extern != 0)) || (look2 != NULL && field -> type -> u.func.is_extern != 0));
				else{
					if(look2 == NULL && field != NULL && field -> type -> kind == FUNCTION && field -> type -> u.func.is_extern != 0);
					else{
						PrintSemErr(3, node -> lineNum, child -> TYPE_ID);
						SemanticError++;
						VarField -> type -> kind = ERROR;
					}
				}
			}
			if(look != NULL){
				if((look2 != NULL && look2 -> type -> kind == FUNCTION && look2 -> type -> u.func.is_extern != 0) || (look2 != NULL && field -> type -> u.func.is_extern != 0));
				else insert_hash(VarField);//nomal insert
			}
			else{
				insert_hash(VarField);
			}
		}
		else if(field != NULL && field -> type -> kind != STRUCTTAG){//func argv insert
			insert_hash(VarField);
		}
		else if(field != NULL && field -> type -> kind == STRUCTTAG){// struct insert
			if(HaveMember(field -> type, child -> TYPE_ID) != NULL)
			{
				
				PrintSemErr(15, node -> lineNum, child -> TYPE_ID);
				SemanticError++;
				VarField -> type -> kind = ERROR;
			}
			
		}
	}
	else {
		Type array = (Type) malloc(sizeof(struct Type_));
		array -> kind = ARRAY;
		int num = child -> brother -> brother -> TYPE_INT;
		array -> u.array.size = num;
		array -> u.array.elem = type;
		return VarDec(child, array, field);
	}
	return VarField;
};

void FunDec(struct Node* node, Type type, unsigned is_extern){
	struct Node* child = node -> child;
	FieldList field = (FieldList) malloc(sizeof(struct FieldList_));
	field -> type = (Type) malloc(sizeof(struct Type_));
	field -> type -> kind = FUNCTION;
	field -> name = (char*) malloc(sizeof(char) * 32);
	field -> type -> u.func.ret = type;
	field -> type -> u.func.argc = 0;
	field -> type -> u.func.argv = NULL;
	field -> type -> u.func.is_extern = is_extern;
	FieldList look = lookup_hash(child -> TYPE_ID);
	if(look != NULL){
		if(look -> type -> kind != FUNCTION || ((look -> type -> u.func.is_extern == 0) && (is_extern == 0))){
			PrintSemErr(4, node -> lineNum, child -> TYPE_ID);
			SemanticError++;
			field -> type -> kind = ERROR;
			type -> kind = ERROR;
			return;
		}
		
	}

	struct Node* brother = child -> brother -> brother;
	strcpy(field -> name, child -> TYPE_ID);
	if(!strcmp(brother -> name, "VarList\0")){
		VarList(brother,field);
	}
	if(look == NULL)	insert_hash(field);
	else{
		if(is_extern != 0){//repeat extern
			if(!FuncArgMatch(look, field)){
				PrintSemErr(19, node -> lineNum, field->name);
				SemanticError++;
				field -> type -> kind = ERROR;
				type -> kind = ERROR;
				return;
			}
		}
		else{//def find extern
			if(!FuncArgMatch(look, field)){
				PrintSemErr(19, node -> lineNum, field -> name);
				SemanticError++;
				field -> type -> kind = ERROR;
				type -> kind = ERROR;
				return;
			}
			look -> type -> u.func.is_extern = 0;
		}
	}
};

void VarList(struct Node* node, FieldList field){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	if(brother != NULL){
		FieldList argv = ParamDec(child, field);
		AddFuncArgv(field, argv);
		struct Node* brother1 = brother -> brother;
		VarList(brother1, field);
	}
	else{
		FieldList argv = ParamDec(child, field);
		AddFuncArgv(field,argv);
	}
};

FieldList ParamDec(struct Node* node, FieldList field){
	Type type = Specifier(node -> child);
	return VarDec(node -> child -> brother, type, field);	
};

void CompSt(struct Node* node, Type type){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	struct Node* brother1 = brother -> brother;
	DefList(brother, NULL);
	StmtList(brother1, type);
};

void StmtList(struct Node* node, Type type){
	struct Node* child = node -> child;
	if(child != NULL){
		struct Node* brother = child -> brother;
		Stmt(child, type);
		StmtList(brother, type);
	}
};

void Stmt(struct Node* node,Type type){
	struct Node* child = node -> child;
	if(!strcmp(child -> name, "Exp\0")){
		Exp(child);
	}
	else if(!strcmp(child -> name, "RETURN\0")){	
		struct Node* brother = child -> brother;
		Type returnType = Exp(brother);
		if(returnType != NULL){
			if(!TypeMatch(type, returnType)){
				PrintSemErr(8, node -> lineNum, NULL);
				SemanticError++;
			}
		}
		
	}
	else if(!strcmp(child -> name, "CompSt\0")){	
		CompSt(child,type);
	}
	else if(!strcmp(child -> name, "IF\0")){
		struct Node* exp1 = child -> brother -> brother;
		struct Node* stmt1 = (child -> brother -> brother -> brother -> brother);//IF LP Exp RP Stmt
		if(stmt1->brother == NULL){
				Type exptype = Exp(exp1);
				if(exptype != NULL && (exptype -> kind != BASIC || exptype -> u.basic != INT)){
					PrintSemErr(7, node -> lineNum,NULL);
					SemanticError++;
				}
				Stmt(stmt1,type);
		}
		else {//IF LP Exp RP Stmt ELSE Stmt
		struct Node* stmt2 = stmt1 -> brother -> brother;
		Type exptype = Exp(exp1);
		if(exptype != NULL && (exptype -> kind != BASIC || exptype -> u.basic != INT)){
			PrintSemErr(7, node -> lineNum, NULL);
			SemanticError++;
		}
		Stmt(stmt1, type);
		Stmt(stmt2, type);
		}
	}
	else if(!strcmp(child -> name, "WHILE\0")){//WHILE LP Exp RP Stmt
		struct Node* exp1 = child -> brother -> brother;
		Type exptype = Exp(exp1);
		if(exptype != NULL && (exptype -> kind != BASIC || exptype -> u.basic != INT)){
			PrintSemErr(7, node -> lineNum, NULL);
			SemanticError++;
		}
		struct Node* stmt1 = exp1 -> brother -> brother;
		Stmt(stmt1,type);
	}

};

void DefList(struct Node* node, FieldList field){
	struct Node*child = node -> child;
	if(child != NULL){
		Def(child, field);
		DefList(child -> brother, field);
	}
};

void Def(struct Node* node, FieldList field){
	struct Node* child = node -> child;
	Type type = Specifier(child);
	DecList(child -> brother, type, field);
};

void DecList(struct Node* node, Type type, FieldList field){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	if(brother == NULL){
		Dec(child, type, field);
	}
	else{
		Dec(child, type, field);
		struct Node* brother2 = brother -> brother;
		DecList(brother2, type, field);
	}
};

void Dec(struct Node* node, Type type, FieldList field){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	if(brother == NULL){
		if(field != NULL && field -> type -> kind == STRUCTTAG){
			FieldList member = VarDec(child, type, field);
			AddStructMember(field, member);
		}
		else{
			VarDec(child,type,field);
		}
	}
	else{
		if(field != NULL && field -> type -> kind == STRUCTTAG){
			PrintSemErr(15, node -> lineNum, NULL);
			SemanticError++;
			FieldList member = VarDec(child, type, field);
			AddStructMember(field, member);
		}
		else {
			struct Node* brother1 = brother -> brother;
			FieldList fi = VarDec(child, type, field);
			Type exptype = Exp(brother1);
			if(exptype != NULL && fi -> type -> kind != ERROR && !TypeMatch(exptype, fi -> type)){
				PrintSemErr(5, node -> lineNum, NULL);
				SemanticError++;
			}
		}
	}
	
};

Type Exp(struct Node* node){
		struct Node* child = node -> child;
		if(!strcmp(child -> name, "LP\0")){//LP Exp RP
			struct Node* brother = child -> brother;
			return Exp(brother);
		}
		else if(!strcmp(child -> name, "ID\0")){
			struct Node* brother = child -> brother;
			FieldList result = lookup_hash(child -> TYPE_ID);
			if(brother == NULL){//ID
				//printf("Exp->ID:%s\n",child->TYPE_ID);
				if(result == NULL || result -> type -> kind == STRUCTTAG || result -> type -> kind == FUNCTION){
					PrintSemErr(1, node -> lineNum,child -> TYPE_ID);
					SemanticError++;
					return NULL;
				}
				return result -> type;
			}
			else if(!strcmp(brother -> name, "LP\0")){
				struct Node*brother1 = brother -> brother;
				if(!strcmp(brother1 -> name, "Args\0")){//ID LP Args RP
					if(result == NULL){
						PrintSemErr(2, node -> lineNum, child -> TYPE_ID);
						SemanticError++;
						return NULL;
					}
					if(result -> type -> kind != FUNCTION){
						PrintSemErr(11, node -> lineNum, child -> TYPE_ID);
						SemanticError++;
						return NULL;
					}		
					FieldList argfield = Args(brother1);
					if(!ArgMatch(result, argfield)){
						PrintSemErr(9, node -> lineNum, result -> name);
						SemanticError++;
						return NULL;
					}
					return result -> type -> u.func.ret;				
				}
				else{//ID LP RP
					if(result == NULL){
						PrintSemErr(2, node -> lineNum, child -> TYPE_ID);
						SemanticError++;
						return NULL;
					}
					if(result -> type -> kind != FUNCTION){
						PrintSemErr(11, node -> lineNum, child -> TYPE_ID);
						SemanticError++;
						return NULL;
					}
					return result -> type -> u.func.ret;
				}			
			}
		}
		else if(!strcmp(child -> name, "Exp\0")){
			Type type1 = Exp(child);
			if(type1 == NULL) return NULL;
			struct Node* brother = child -> brother;
			if(!strcmp(brother -> name, "LB\0")){//Exp LB Exp RB
				if(type1 -> kind != ARRAY){
					PrintSemErr(10, node -> lineNum, NULL);
					SemanticError++;
					return NULL;
				}
				struct Node* brother = child -> brother;
				Type type2 = Exp(brother -> brother);
				if(type2 == NULL) return NULL;
				else if(type2 -> kind != BASIC || type2 -> u.basic != INT){
					PrintSemErr(12, node -> lineNum, NULL);
					SemanticError++;
					return NULL;
				}
				return type1 -> u.array.elem;
			}
			else if(!strcmp(brother -> name, "DOT\0")){//Exp DOT ID 
				if(type1 -> kind != STRUCTURE){
					PrintSemErr(13, node -> lineNum, NULL);
					SemanticError++;
					return NULL;
				}
				FieldList member = HaveMember(type1 -> u.structure -> type, brother -> brother -> TYPE_ID);
				if(member == NULL){
					PrintSemErr(14, node -> lineNum, brother -> brother -> TYPE_ID);
					SemanticError++;
					return NULL;
				}
				return member -> type;
			}
			else if(!strcmp(brother -> name, "ASSIGNOP\0")){//Exp ASSIGNOP Exp
				Type type2 = Exp(brother -> brother);
				if(type1 != NULL && type2 != NULL && !TypeMatch(type1, type2)){
					PrintSemErr(5, node -> lineNum, NULL);
					SemanticError++;
					return NULL;
				}
				else if(!((!strcmp(child -> child -> name, "ID\0") && (child -> child -> brother == NULL)) ||
						(!strcmp(child -> child -> name, "Exp\0") && !strcmp(child -> child -> brother -> name, "DOT\0")) ||
						(!strcmp(child -> child -> name, "Exp\0") && (!strcmp(child -> child -> brother -> name, "LB\0")))
					)){
						PrintSemErr(6, node -> lineNum, NULL);
						SemanticError++;
						return NULL;
					}
				return type1;
			}
			else{
			/*
			Exp STAR Exp
			Exp DIV Exp
			Exp PLUS Exp
			Exp MINUS Exp
			Exp RELOP Exp
			Exp AND Exp
			Exp OR Exp			
			*/
			struct Node* brother = child -> brother;
			Type type2 = Exp(brother -> brother);
			if(type2 == NULL) { return NULL; }
			if(!TypeMatch(type1, type2)){
				PrintSemErr(7, node -> lineNum, NULL);
				SemanticError++;
				return NULL;
			}
			if((!strcmp(brother -> name, "AND\0")) || (!strcmp(brother -> name, "OR\0"))){
				if(type1 -> kind != BASIC || type1 -> u.basic != INT){
					PrintSemErr(7, node -> lineNum, NULL);
					SemanticError++;
					return NULL;
				}
				Type toret = (Type) malloc(sizeof(struct Type_));
				toret -> kind = BASIC;
				toret -> u.basic = INT;
				return toret;
			}
			else if(!strcmp(brother -> name, "RELOP\0")){
				Type toret = (Type) malloc(sizeof(struct Type_));
				toret -> kind = BASIC;
				toret -> u.basic = INT;
				return toret;
			}
			return type1;
			}
		}
		else if(!strcmp(child -> name, "MINUS\0")){//MINUS Exp
			struct Node* brother = child -> brother;
			Type exptype = Exp(brother);
			if(exptype != NULL && (exptype -> kind != BASIC)){
				PrintSemErr(7, node -> lineNum, NULL);
				SemanticError++;
			}
			return exptype;
		}
		else if(!strcmp(child -> name, "NOT\0")){//NOT Exp
			struct Node* brother = child -> brother;
			Type exptype = Exp(brother);
			if(exptype != NULL && (exptype -> kind != BASIC || exptype -> u.basic != INT)){
				PrintSemErr(7, node -> lineNum, NULL);
				SemanticError++;
			}
			return exptype;
		}
		else if(!strcmp(child -> name, "INT\0")){//INT
			Type inttype = (Type) malloc(sizeof(struct Type_));
			inttype -> kind = BASIC;
			inttype -> u.basic = INT;
			return inttype;
		}
		else if(!strcmp(child -> name, "FLOAT\0")){//FLOAT
			Type floattype = (Type) malloc(sizeof(struct Type_));
			floattype -> kind = BASIC;
			floattype -> u.basic = FLOAT;
			return floattype;
		}
};

FieldList Args(struct Node* node){
	struct Node* child = node -> child;
	struct Node* brother = child -> brother;
	FieldList arg = (FieldList) malloc(sizeof(struct FieldList_));
	if(brother == NULL){//Exp
		arg -> type = Exp(child);
		arg -> tail = NULL;
		arg -> name = NULL;
	}
	else{//Exp COMMA Args
		struct Node* brother1 = brother -> brother;
		arg -> type = Exp(child);
		arg -> tail = Args(brother1);
		arg -> name = NULL;
	}
	return arg;
};
