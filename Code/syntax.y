%locations
%{
#include <stdio.h>
#include "Node.h"
#define YYSTYPE struct Node*
int yylex(void);
int yyerror(char* s);
int SyntaxError = 0;
%}

/* declared tokens */
%token INT FLOAT ID
%token SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE 
%right ASSIGNOP
%left OR
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT  
%left DOT 
%left LB RB 
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
/*High-level Definitions*/
Program : ExtDefList { $$ = creatNode($1, "Program\0", @$.first_line); root = $$; };

ExtDefList : ExtDef ExtDefList { $$ = creatNode($1, "ExtDefList\0", @$.first_line); $1 -> brother = $2; }
| { $$ = creatNode(NULL, "ExtDefList\0", @$.first_line); $$ -> epsilon = 1; };

ExtDef : Specifier ExtDecList SEMI { $$ = creatNode($1, "ExtDef\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; /*printf("111222  %d\n", @$.first_line);*/}
| Specifier SEMI { $$ = creatNode($1, "ExtDef\0", @$.first_line); $1 -> brother = $2; }
| Specifier FunDec CompSt { $$ = creatNode($1, "ExtDef\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Specifier FunDec SEMI {$$ = creatNode($1, "ExtDef\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3;}
| error SEMI { SyntaxError += 1; }
| Specifier error SEMI { SyntaxError += 1; }
| error Specifier SEMI { SyntaxError += 1; };

ExtDecList : VarDec { $$ = creatNode($1, "ExtDecList\0", @$.first_line); }
| VarDec COMMA ExtDecList { $$ = creatNode($1, "ExtDecList\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }

/*Specifiers*/
Specifier : TYPE { $$ = creatNode($1, "Specifier\0", @$.first_line); }
| StructSpecifier { $$ = creatNode($1, "Specifier\0", @$.first_line); };

StructSpecifier : STRUCT OptTag LC DefList RC { $$ = creatNode($1, "StructSpecifier\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; $4 -> brother = $5; }
| STRUCT Tag { $$ = creatNode($1, "StructSpecifier\0", @$.first_line); $1 -> brother = $2; };

OptTag : ID { $$ = creatNode($1, "OptTag\0", @$.first_line); }
| { $$ = creatNode(NULL, "OptTag\0", @$.first_line); $$ -> epsilon = 1; };

Tag : ID { $$ = creatNode($1, "Tag\0", @$.first_line); };

/*Declarators*/
VarDec : ID { $$ = creatNode($1, "VarDec\0", @$.first_line); }
| VarDec LB INT RB { $$ = creatNode($1, "VarDec\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; }
| VarDec LB error RB { SyntaxError += 1; };
| VarDec LB INT RB error { SyntaxError += 1; }

FunDec : ID LP VarList RP { $$ = creatNode($1, "FunDec\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; }
| ID LP RP { $$ = creatNode($1, "FunDec\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| ID LP error RP { SyntaxError += 1; }
| error LP VarList RP { SyntaxError += 1; };

VarList : ParamDec COMMA VarList { $$ = creatNode($1, "VarList\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| ParamDec { $$ = creatNode($1, "VarList\0", @$.first_line); };

ParamDec : Specifier VarDec { $$ = creatNode($1, "ParamDec\0", @$.first_line); $1 -> brother = $2; };

/*Statements*/
CompSt : LC DefList StmtList RC { $$ = creatNode($1, "CompSt\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; /*printf("1111 %d\n", @$.first_line);*/ };

StmtList : Stmt StmtList { $$ = creatNode($1, "StmtList\0", @$.first_line); $1 -> brother = $2; }
| { $$ = creatNode(NULL, "StmtList\0", @$.first_line); $$ -> epsilon = 1; };

Stmt : Exp SEMI { $$ = creatNode($1, "Stmt\0", @$.first_line); $1 -> brother = $2; }
| CompSt { $$ = creatNode($1, "Stmt\0", @$.first_line); }
| RETURN Exp SEMI { $$ = creatNode($1, "Stmt\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| IF LP Exp RP Stmt { $$ = creatNode($1, "Stmt\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; $4 -> brother = $5; }
| IF LP Exp RP Stmt ELSE Stmt { $$ = creatNode($1, "Stmt\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; $4 -> brother = $5; $5 -> brother = $6; $6 -> brother = $7; }
| WHILE LP Exp RP Stmt { $$ = creatNode($1, "Stmt\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; $4 -> brother = $5; }
| IF error Exp RP Stmt { SyntaxError += 1; }
| IF LP error RP Stmt { SyntaxError += 1; }
| IF LP Exp error Stmt { SyntaxError += 1; }
| WHILE error Exp RP Stmt { SyntaxError += 1; }
| WHILE LP error RP Stmt { SyntaxError += 1; }
| WHILE LP Exp error Stmt { SyntaxError += 1; }
| error SEMI { SyntaxError += 1; yyerrok; }
| Exp error SEMI { SyntaxError += 1; }
| Exp error { SyntaxError += 1; }
| RETURN error SEMI { SyntaxError += 1; }
| RETURN Exp error { SyntaxError += 1; };

/*Local Definitions*/
DefList : Def DefList { $$ = creatNode($1, "DefList\0", @$.first_line); $1 -> brother = $2; }
| { $$ = creatNode(NULL, "DefList\0", @$.first_line); $$ -> epsilon = 1; /*printf("11122211   %d\n", @$.first_line);*/ };

Def : Specifier DecList SEMI { $$ = creatNode($1, "Def\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; /*printf("111222  %d\n", @$.first_line);*/ }
| Specifier error SEMI { SyntaxError += 1; }
| Specifier DecList error { SyntaxError += 1; };

DecList : Dec { $$ = creatNode($1, "DecList\0", @$.first_line); }
| Dec COMMA DecList { $$ = creatNode($1, "DecList\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; };

Dec : VarDec { $$ = creatNode($1, "Dec\0", @$.first_line); }
| VarDec ASSIGNOP Exp { $$ = creatNode($1, "Dec\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; };

/*Expressions*/
Exp : LP Exp RP { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| ID LP Args RP { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; }
| ID LP RP { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp LB Exp RB { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; $3 -> brother = $4; }
| Exp DOT ID { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| MINUS Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; }
| NOT Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; }
| Exp STAR Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp DIV Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp PLUS Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp MINUS Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp RELOP Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp AND Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp OR Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp ASSIGNOP Exp { $$ = creatNode($1, "Exp\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| ID { $$ = creatNode($1, "Exp\0", @$.first_line); }
| INT { $$ = creatNode($1, "Exp\0", @$.first_line); }
| FLOAT { $$ = creatNode($1, "Exp\0", @$.first_line); }
| LP error RP { SyntaxError += 1; }
| ID LP error RP { SyntaxError += 1; }
| Exp LB error RB{ SyntaxError += 1; }
| MINUS error { SyntaxError += 1; }
| NOT error { SyntaxError += 1; }
| Exp STAR error { SyntaxError += 1; }
| Exp DIV error { SyntaxError += 1; }
| Exp PLUS error { SyntaxError += 1; }
| Exp MINUS error { SyntaxError += 1; }
| Exp RELOP error { SyntaxError += 1; }
| Exp AND error { SyntaxError += 1; }
| Exp OR error { SyntaxError += 1; }
| Exp ASSIGNOP error { SyntaxError += 1; }
;

Args : Exp COMMA Args { $$ = creatNode($1, "Args\0", @$.first_line); $1 -> brother = $2; $2 -> brother = $3; }
| Exp { $$ = creatNode($1, "Args\0", @$.first_line); };


%%
#include "lex.yy.c"
#include "semantic.h"
#include "HashMap.h"
int yylex();

int main(int argc, char** argv) {
    //printf("11111\n");
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
	    return 1;
    }
    LexError = 0, SyntaxError = 0;
    printf("Parse begin!\n");
    yyrestart(f);
    yyparse();
    printf("Parse end!\n");
    if(LexError == 0 && SyntaxError == 0){
        printf("Semantic anyasis begin !\n");
	    semantic_anyasis(root);
        printf("Semantic anyasis end!\n");
	    //PrintMap();
        printf("Build intercodes begin !\n");
		if (argc >= 3) inter_code(argv[2], root);
        else inter_code(NULL, root);
        printf("Build intercodes end !\n");
	}
    return 0;

}

int yyerror(char* msg) {
    printf("Error type B at Line %d: %s\n", yylineno, yytext);
    //return 0;
}
