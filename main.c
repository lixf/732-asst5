#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "simple.h"
#include "eval.h"

int yyparse ();

extern FILE *yyin;

ast_t *program = NULL;
int numerrors = 0;

int main ( int argc, char* argv[] ) {
  int ret;
  state_t *state;
  int argptr =1;

  if(argc > 1){
      if(strcmp(argv[1], "-d") == 0){
	  debug_eval(1);
	  argptr++;
      } else {
	  debug_eval(0);
      }
    yyin = fopen(argv[argptr], "r");
  } else
    yyin = stdin;

  inityylex();


  ret = yyparse();

  if( (ret == 0) && (numerrors == 0)){
    state = (state_t *)malloc(sizeof(state_t));
    state->tbl = NULL;
    state->mem = NULL;
    eval_stmts(program, state);
    return 0;
  } else {
    return -1;
  }

};
