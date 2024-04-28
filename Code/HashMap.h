#ifndef HASHMAP
#define HASHMAP
#include <stdbool.h>
#include "intercode.h"

#define HASHMAP_SIZE 0x3fff
#define INT 0
#define FLOAT 1

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct HashNode_* HashNode;
typedef struct Operand_* Operand;

extern HashNode HashMap[HASHMAP_SIZE];

struct Type_{
	enum{BASIC, ARRAY, STRUCTURE, STRUCTTAG, FUNCTION, ERROR} kind;
	union{
		int basic;
		struct { Type elem; int size; }array;
		FieldList structure;
		FieldList structmember;
		struct {Type ret; int argc; FieldList argv; unsigned is_extern; } func;
	}u;
};

struct FieldList_{
	char* name;
	Operand op;
	Type type;
	FieldList tail;
};

struct HashNode_{
	FieldList field;
	HashNode next;
};

unsigned hash_pjw(char* name);

void init_hash();

HashNode insert_hash(FieldList field);

FieldList lookup_hash(char* name);

void AddStructMember(FieldList field,FieldList member);
void AddFuncArgv(FieldList field,FieldList Argv);
void printType(Type type);
void printhashNode(FieldList Node);
void PrintMap();
void PrintSemErr(unsigned type,unsigned linenum,char *elem);
bool TypeMatch(Type a,Type b);
bool ArgMatch(FieldList func,FieldList arg);
bool FuncArgMatch(FieldList func1,FieldList func2);
FieldList HaveMember(Type type,char *ID);
void CheckUndefFunc();
int getsize(Type type);
int get_array_size(struct Node* Exp);
int get_struct_offset(Type type, char* name);
#endif
