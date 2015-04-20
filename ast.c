//
//  ast.c
//  taint_hw
//
//  Created by David Brumley on 10/30/11.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"



ast_t *mk_int(const int x)
{
    ast_t *res = (ast_t *)malloc(sizeof(ast_t));
    res->tag = int_ast;
    res->info.integer = x;
    return res;
}

ast_t* mk_str(const char *str)
{
    ast_t *res= (ast_t *)malloc(sizeof(ast_t));
    res->tag = str_ast;
    res->info.string = strdup(str);
    return res;
}

ast_t *mk_var(const char *var)
{
    ast_t *res = (ast_t *)malloc(sizeof(ast_t));
    res->tag = var_ast;
    res->info.varname = strdup(var);
    return res;
}

ast_t *mk_node (const ast_kind tag, ast_list_t *args)
{
    ast_t *res = (ast_t*) malloc(sizeof(ast_t));
    res->tag = node_ast;
    res->info.node.tag = tag;
    res->info.node.arguments = args;
    return res;
}

ast_list_t* cons ( ast_t* e, ast_list_t* r ) {
    ast_list_t* res = (ast_list_t*) malloc(sizeof(ast_list_t));
    res->elem = e;
    res->next = r;
    return res;
};


ast_list_t *rev(ast_list_t *head) {
  ast_list_t *next = NULL;
  ast_list_t *prev = NULL;

  while(head != NULL){
    next = head->next;
    head->next = prev;
    prev=  head;
    head = next;
  }
  return prev;
}



