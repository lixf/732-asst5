#ifndef EVAL_H
#define EVAL_H
#include "tables.h"

typedef struct state_t {
    varctx_t *tbl;
    memctx_t *mem;
} state_t;

void debug_eval(int);
state_t * eval_stmts(ast_t *program, state_t *state);

#endif
