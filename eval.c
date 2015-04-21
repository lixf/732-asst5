#include "ast.h"
#include "tables.h"
#include "eval.h"
#include <assert.h>

#define TAINTED 1 
#define NOT_TAINTED 0

#define dbg_printf(...) \
    do { if (eval_debug) fprintf(stderr, __VA_ARGS__); } while (0)

int eval_debug = 0;

void debug_eval(int val)
{
    eval_debug = val;
}

value_t eval_exp(ast_t *e, varctx_t *tbl, memctx_t *mem, exp_info *ei)
{
    value_t ret;
    switch(e->tag){
        case int_ast:
	        dbg_printf("[Debug][eval_exp][int_ast]\n");
            return e->info.integer;
            break;
        case var_ast:
            dbg_printf("[Debug][eval_exp][var_ast][varname: %s]\n", e->info.varname);
            return lookup_var(e->info.varname, tbl,ei);
            break;
        case node_ast: {
	        dbg_printf("[Debug][eval_exp][node_ast]\n");
        switch(e->info.node.tag){
	        case MEM:
	          return load(eval_exp(e->info.node.arguments->elem, tbl,mem,ei), mem);
	          break;
	        case PLUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei) + 
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei);
	          break;
	        case MINUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei) -
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei);
	          break;
	        case DIVIDE:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei) /
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei);
	          break;
	        case TIMES:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei) *
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei);
	          break;

	        case EQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) == 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	          break;
	        case NEQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) != 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	          break;
	        case GT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) > 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	            break;
	        case LT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) <
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	            break;
	        case LEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) <= 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	            break;
	        case GEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) >=
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	            break;
	        case AND:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) && 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	            break;
	        case OR:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei) ||
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei));
	          break;
	        case NEGATIVE:
	          return -(eval_exp(e->info.node.arguments->elem,tbl,mem,ei));
	        case NOT:
	          return !(eval_exp(e->info.node.arguments->elem,tbl,mem,ei));
            case IFE:
              return  eval_exp(e->info.node.arguments->elem,tbl,mem,ei)?
                      eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei):
                      eval_exp(e->info.node.arguments->next->next->elem,tbl,mem,ei);
	        case READINT:
	          printf("> ");
	          scanf("%d", &ret);
	          return ret;
	          break;
	        case READSECRETINT:
	          printf("# ");
	          scanf("%d", &ret);
              dbg_printf("[Debug][eval_exp] Just read SECRET \n");
              ei->tainted = TAINTED;
              return ret;
	          break;
	        default:
	          assert(0); // Unknown/unhandled op.
	        }
        }
        default:
            assert(0);
    }
}

state_t* eval_stmts(ast_t *p, state_t *state) {
    ast_list_t *stmts;
    ast_list_t *ip;
    ast_t *t1, *t2;
    ast_t *s;
    value_t v;
    int tainted = 0;
    exp_info ei;
    ei.tainted = 0;
    ei.ti = NULL;
    if (state->tbl == NULL) {
        // If we have no intitial, context, create one.
        state->tbl = newvar("INITIAL_CONTEXT", NULL, 0);
    }

    assert(p != NULL);
    assert(p->info.node.tag == SEQ);
    ip = p->info.node.arguments;
    while(ip != NULL) {
	    s = ip->elem;
        ei.tainted = 0;
        ei.ti = NULL;
	    switch(s->info.node.tag){
	        case ASSIGN:
                /* the lhs */
	            t1 = s->info.node.arguments->elem;
                dbg_printf("[Debug] ASSIGNING to %s\n", t1->info.string);
	            /* the rhs */
	            t2 = s->info.node.arguments->next->elem;
	            v = eval_exp(t2, state->tbl, state->mem, &ei);
	            switch(t1->tag){
	                case var_ast:
                        dbg_printf("[Debug][ASSIGN][VAR AST] RHS Tainted: %s\n",
                                (ei.tainted == TAINTED) ? "YES" : "NO");
                        update_var(t1->info.string, v, state->tbl, ei.tainted);
	        	        break;
	                case node_ast:
	                    dbg_printf("[Debug][ASSIGN][NODE AST]\n");
	        	        assert(t1->info.node.tag == MEM);
                        ei.tainted = 0;
	        	        state->mem = store(eval_exp(t1->info.node.arguments->elem,
                              state->tbl, state->mem, &ei), v, state->mem);
	        	        break;
	                default:
	        	        assert(0);
	            }
	            break;
	        case PRINT:
	            switch(s->info.node.arguments->elem->tag){
	                case str_ast:
                        dbg_printf("[Debug][Print][str_ast]\n");
                        fprintf(stderr, "Tainted variable: None\n");
	        	        fprintf(stderr, "%s\n", s->info.node.arguments->elem->info.string);
	        	        break;
	                default:
                        dbg_printf("[Debug][Print][default]\n");
                        v = eval_exp(s->info.node.arguments->elem,
                                     state->tbl,
                                     state->mem,
                                     &ei);
                        if (ei.tainted == TAINTED) {
                            // TODO Extract all tainted variables (and memory locations)
                            // from this expression.
                            fprintf(stderr, "Tainted variable: ");
                            print_tainted_items(&ei);
                            fprintf(stderr, "\n");
	        	            fprintf(stdout, "<secret>\n");
                        } else {
                            fprintf(stderr, "Tainted variable: None\n");
	        	            fprintf(stdout, "%u\n", v);
                        }
	        	        break;
	            }
	            break;
	        case IF:
                if(eval_exp(s->info.node.arguments->elem, state->tbl, state->mem, &ei)){
	        	    state = eval_stmts(s->info.node.arguments->next->elem, state);
	            } else {
	        	    state = eval_stmts(s->info.node.arguments->next->next->elem, state);
                } 
	            break;
	        case SEQ:
	            state = eval_stmts(s->info.node.arguments->next->elem, state);
	            break;
	        case ASSERT:
	            if(eval_exp(s->info.node.arguments->elem, state->tbl,state->mem, &ei) ==0){
	        	    printf("Assert failed!\n");
	            }
	            break;
	        default:
	            printf("Unknown statement type\n");
	            assert(0);
	            break;
	    }
	    ip = ip->next;
    }
    return state;
}
