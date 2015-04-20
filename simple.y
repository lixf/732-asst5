%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

  static int lineno = 1;
#include "simple.yy.c"


extern FILE *yyin;


static int numerrors;

static void  yyerror(char *msg){
    int i;
    fflush(stdout);
    fprintf(stderr, "\n*** Error line %d\n", yylloc.first_line);
    const char *line = GetLineNumbered(yylloc.first_line);    
    if(line == NULL) return;
    fprintf(stderr, "%s\n", line);
    for(i = 1; i <= yylloc.last_column; i++){
	fprintf(stderr, "%c", i>=yylloc.first_column ? '^' : ' ');
    }
    fprintf(stderr, "\n");
   numerrors ++;
}

extern ast_t *program;

%}

%union {
  char *stringConstant;
  unsigned integerConstant;
  char *identifier;
  ast_t * ast;
  ast_list_t *ast_list;
}

%token T_Print T_ReadInteger T_Assign T_Assert
%token T_LessEqual T_GreaterEqual T_Equal T_NotEqual
%token T_QM T_Colon
%token T_And T_Or T_If T_Else  T_Mem T_ReadSecretInt
%token <identifier>T_Identifier
%token <stringConstant>T_StringConstant 
%token <integerConstant>T_IntConstant



/* %type <stringConstant> id */
/* %type <ast> exp int real string name */
/* %type <ast_list> expl names */

%type <ast> Stmt AssignStmt LValue Exp BoolExpr Constant
%type <ast> IfThenElseStmt PrintStmt  AssertStmt 
%type <ast_list>  StmtList

%nonassoc '='
%right T_QM T_Colon
%left T_Or
%left T_And 
%nonassoc T_Equal T_NotEqual
%nonassoc T_LessEqual T_GreaterEqual '<' '>'
%left '+' '-'
%left '*' '/' '%'
%right T_UnaryMinus '!' 
%right '[' '.' 
%nonassoc T_Lower_Than_Else ')'
%nonassoc T_Else

%%

Start : StmtList { @1; program = mk_node(SEQ, rev($1));  }

;

StmtList : StmtList Stmt { $$ = cons($2, $1); }
      | /* empty */ { $$ = NULL; } 
      ;

Stmt : IfThenElseStmt  {  $$ = $1;}
       | AssertStmt ';' { $$ = $1; }
       | PrintStmt ';' { $$ = $1;}
       | AssignStmt ';' { $$ = $1; }
       | error ';'       { yyclearin; yyerrok; }
;

AssignStmt : LValue T_Assign Exp 
	   { $$ = mk_node(ASSIGN, cons($1, cons($3, NULL))); }
                   | /* empty */ { $$ = NULL; }
;

LValue : T_Identifier { $$ = mk_var($1); } 
        | T_Mem '[' Exp ']'  { $$ = mk_node(MEM, cons($3, NULL)); }
;

Exp : LValue  { $$ = $1; }
	| 	Constant { $$ = $1; }
	| 	Exp '+' Exp { $$ = mk_node(PLUS, cons($1,cons($3, NULL))); } 
	| 	Exp '-' Exp { $$ = mk_node(MINUS, cons($1, cons($3, NULL))); }
	| 	Exp '/' Exp { $$ = mk_node(DIVIDE, cons($1, cons($3, NULL))); }
	| 	Exp '*' Exp { $$ = mk_node(TIMES, cons($1, cons($3, NULL))); }
	| 	Exp T_Equal Exp { $$ = mk_node(EQ, cons($1, cons($3, NULL))); } 
	| 	Exp T_NotEqual Exp 
	{ $$ = mk_node(NEQ, cons($1, cons($3, NULL))); }
	| 	Exp '<' Exp    { $$ = mk_node(LT,cons($1, cons($3, NULL))); } 
	| 	Exp '>' Exp { $$ = mk_node(GT, cons($1, cons( $3, NULL))); } 
	| 	Exp T_LessEqual Exp 
        { $$ = mk_node(LEQ, cons($1, cons($3, NULL))); } 
	| 	Exp T_GreaterEqual Exp 
	{ $$ = mk_node(GEQ, cons($1, cons($3, NULL))); }
	| 	Exp T_And Exp { $$ = mk_node(AND, cons($1, cons($3, NULL))); }
	| 	Exp T_Or Exp { $$ = mk_node(OR, cons($1, cons($3, NULL))); }
        | Exp T_QM Exp T_Colon Exp { $$ =
                      mk_node(IFE, cons($1, cons($3, cons($5, NULL))));}
	| '(' 	Exp ')' { $$ = $2; } 
	| '-' 	Exp %prec T_UnaryMinus 
	{ $$ = mk_node(NEGATIVE, cons($2, NULL)); }
	| '!' 	Exp  { $$ = mk_node(NOT, cons($2, NULL)); } 
	| 	T_ReadInteger '(' ')' { $$ = mk_node(READINT, NULL); }
        |	T_ReadSecretInt '(' ')' 
                    { $$ = mk_node(READSECRETINT,NULL); }
;

BoolExpr : Exp  { $$ = $1; }
;

/* ExpList : ExpList ',' Exp { $$ = cons($3, $1); } */
/*          | Exp { $$ = cons($1, NULL); } */
/* ; */

Constant : T_IntConstant { $$ = mk_int($1);}
          | T_StringConstant { $$ = mk_str($1);}
;

IfThenElseStmt : T_If '(' BoolExpr ')' '{' StmtList '}' T_Else '{'
                  StmtList '}'
{ $$ = mk_node(IF, cons($3, cons(mk_node(SEQ, rev($6)), 
                            cons(mk_node(SEQ, rev($10)),NULL)))); }
 ;

PrintStmt : T_Print '(' Exp ')'  { $$ = mk_node(PRINT, cons($3, NULL));} 
;


AssertStmt : T_Assert '(' BoolExpr ')' 
	   { $$ = mk_node(ASSERT, cons($3, NULL)); }
;

%%

