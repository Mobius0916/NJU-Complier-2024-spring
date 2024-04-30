%{
    #include "syntax.tab.h"
    #include"Node.h"
    #include <stdio.h>
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    extern unsigned LexError;
%}

%option yylineno

letter [a-zA-Z]
digit [0-9]
INT 0|[1-9]{digit}*
FLOAT {INT}\.{digit}+
ID [_a-zA-Z][0-9_a-zA-Z]*
ANNOATE "//"
ANNOATE2 "/*"
SEMI ;
COMMA ,
ASSIGNOP =
RELOP (>)|(<)|(>=)|(<=)|(==)|(!=)
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT !
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
%%
\n { yycolumn = 1;}
{ANNOATE} {
    char c = input();
    while(c != '\n'){
        c = input();
    }
}
{ANNOATE2} {
    char c1 = input(), c2 = input();
    while(c1 != '*' || c2 != '/'){
        c1 = c2; c2 = input();
        if(c2 == '\0'){ 
            printf("Error type A at Line %d: Missing */.\n",yylineno);
            break;
        }
    }
}

{SEMI} {
    struct Node* tmp = creatNode(NULL, "SEMI\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return SEMI;
}
{COMMA} {
    struct Node* tmp = creatNode(NULL, "COMMA\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return COMMA;
}
{ASSIGNOP} {
    struct Node* tmp = creatNode(NULL, "ASSIGNOP\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return ASSIGNOP;
}
{RELOP} {
    struct Node* tmp = creatNode(NULL, "RELOP\0", 0);
    tmp -> is_token = 1;
    strcpy(tmp -> RELOP_, yytext);
    yylval = tmp;
    return RELOP;
}
{PLUS} {
    struct Node* tmp = creatNode(NULL, "PLUS\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return PLUS;
}
{MINUS} {
    struct Node* tmp = creatNode(NULL, "MINUS\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return MINUS;

}
{STAR} {
    struct Node* tmp = creatNode(NULL, "STAR\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return STAR;
}
{DIV} {
    struct Node* tmp = creatNode(NULL, "DIV\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return DIV;
}
{AND} {
    struct Node* tmp = creatNode(NULL, "AND\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return AND;
}
{OR} {
    struct Node* tmp = creatNode(NULL, "OR\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return OR;
}
{DOT} {
    struct Node* tmp = creatNode(NULL, "DOT\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return DOT;
}
{NOT} {
    struct Node* tmp = creatNode(NULL, "NOT\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return NOT;
}
{TYPE} {
    struct Node* tmp;
    if(!strcmp(yytext,"int\0")){
	    tmp = creatNode(NULL,"TYPE",0);
	    tmp->is_type=1;
	}
    else{
	    tmp = creatNode(NULL,"TYPE",0);
	    tmp->is_type=2;
    }
    tmp -> is_token = 1;
    yylval = tmp;
    return TYPE;
}
{LP} {
    struct Node* tmp = creatNode(NULL, "LP\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return LP;
}
{RP} {
    struct Node* tmp = creatNode(NULL, "RP\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return RP;
}
{LB} {
    struct Node* tmp = creatNode(NULL, "LB\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return LB;
}
{RB} {
    struct Node* tmp = creatNode(NULL, "RB\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return RB;
}
{LC} {
    struct Node* tmp = creatNode(NULL, "LC\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return LC;
}
{RC} {
    struct Node* tmp = creatNode(NULL, "RC\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return RC;
}
{STRUCT} {
    struct Node* tmp = creatNode(NULL, "STRUCT\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return STRUCT;
}
{RETURN} {
    struct Node* tmp = creatNode(NULL, "RETURN\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return RETURN;
}
{IF} {
    struct Node* tmp = creatNode(NULL, "IF\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return IF;
}
{ELSE} {
    struct Node* tmp = creatNode(NULL, "ELSE\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return ELSE;
}
{WHILE} {
    struct Node* tmp = creatNode(NULL, "WHILE\0", 0);
    tmp -> is_token = 1;
    yylval = tmp;
    return WHILE;
}
{INT} {
    struct Node* tmp = creatNode(NULL, "INT\0", 0);
    tmp -> is_token = 1;
    tmp -> TYPE_INT = atoi(yytext);
    yylval = tmp;
    return INT;
}
{FLOAT} {
    struct Node* tmp = creatNode(NULL, "FLOAT\0", 0);
    tmp -> is_token = 1;
    tmp -> TYPE_FLOAT = atof(yytext);
    yylval = tmp;
    return FLOAT;
}
{ID} {
    struct Node* tmp = creatNode(NULL, "ID\0", 0);
    tmp -> is_token = 1;
    strcpy(tmp -> TYPE_ID, yytext);
    yylval = tmp;
    return ID;
}
" " {}
\r {}
\t {}
. {
    LexError += 1;
    printf("Error type A at Line %d: Mysterious charaters \'%s\'.\n", yylineno, yytext);
}
%%
