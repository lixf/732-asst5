//
//  ast.h
//  taint_hw
//
//  Created by David Brumley on 10/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef taint_hw_ast_h
#define taint_hw_ast_h

typedef enum { SEQ, ASSIGN, MEM, PLUS, MINUS, DIVIDE, EQ, NEQ, GT, LT,
	       LEQ, GEQ, AND, OR, NEGATIVE, NOT, IFE, READINT, IF, PRINT,
	       GOTO, ASSERT,LABEL, BLOCK, READSECRETINT, TIMES} ast_kind;

static char *ast_names [] = {"SEQ", "ASSIGN", "MEM", "PLUS", "MINUS",
			     "DIVIDE", "EQ", "NEQ", "GT", "LT", "LEQ",
			     "GEQ", "AND", "OR", "NEGATIVE", "NOT", "IFE",
			     "READINT", "IF", "PRINT", "GOTO",
			     "ASSERT","LABEL", "BLOCK",
			     "READSECRETINT", "TIMES"};  


typedef struct ast_t {
    enum {int_ast, str_ast, var_ast, node_ast } tag;
    union {
        int integer;
        char *string;
        char *varname;
        struct {    
            ast_kind tag;
            struct ast_list_t *arguments;
        } node;
    } info;
} ast_t;


typedef struct ast_list_t {
    ast_t* elem;
    struct ast_list_t *next;
} ast_list_t;

ast_t *mk_int(const int x);
ast_t* mk_str(const char *str);
ast_t *mk_var(const char *var);
ast_t *mk_node (const ast_kind tag, ast_list_t *args);
ast_list_t* cons(ast_t *e, ast_list_t *r);
ast_list_t *rev(ast_list_t *r);

#endif
