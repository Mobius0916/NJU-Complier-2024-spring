#ifndef SEMANTIC
#define SEMANTIC
#include "Node.h"
#include "HashMap.h"
extern unsigned SemanticError;
void semantic_anyasis(struct Node *node);

void Program(struct Node*node);

void ExtDefList(struct Node*node);

void ExtDef(struct Node*node);

void ExtDecList(struct Node*node,Type type);

Type Specifier(struct Node*node);

void StructSpecifier(struct Node*node,Type type);

char* OptTag(struct Node*node);

char* Tag(struct Node*node);

FieldList VarDec(struct Node*node,Type type,FieldList field);

void FunDec(struct Node*node,Type type,unsigned is_extern);

void VarList(struct Node*node,FieldList field);

FieldList ParamDec(struct Node*node,FieldList field);

void CompSt(struct Node*node,Type type);

void StmtList(struct Node*node,Type type);

void Stmt(struct Node*node,Type type);

void DefList(struct Node*node,FieldList field);

void Def(struct Node*node,FieldList field);

void DecList(struct Node*node,Type type,FieldList field);

void Dec(struct Node*node,Type type,FieldList field);

Type Exp(struct Node*node);

FieldList Args(struct Node*node);



#endif
