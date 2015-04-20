#include "ast.h"
#include "tables.h"
#include "eval.h"
#include <assert.h>

#define TAINTED 1 
#define NOT_TAINTED 0

int eval_debug = 0;


void debug_eval(int val)
{
    eval_debug = val;
}

value_t eval_exp(ast_t *e, varctx_t *tbl, memctx_t *mem)
{
    value_t ret;
    switch(e->tag){
        case int_ast: return e->info.integer; break;
        case var_ast: return lookup_var(e->info.varname, tbl); break;
        case node_ast: {
	    switch(e->info.node.tag){
	        case MEM:
	          return load(eval_exp(e->info.node.arguments->elem, tbl,mem), mem);
	          break;
	        case PLUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem) + 
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem);
	          break;
	        case MINUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem) -
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem);
	          break;
	        case DIVIDE:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem) /
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem);
	          break;
	        case TIMES:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem) *
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem);
	          break;

	        case EQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem) == 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	          break;
	        case NEQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem) != 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	          break;
	        case GT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) > 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	            break;
	        case LT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) <
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	            break;
	        case LEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) <= 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	            break;
	        case GEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) >=
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	            break;
	        case AND:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) && 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	            break;
	        case OR:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem) ||
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem));
	          break;
	        case NEGATIVE:
	          return -(eval_exp(e->info.node.arguments->elem,tbl,mem));
	        case NOT:
	          return !(eval_exp(e->info.node.arguments->elem,tbl,mem));
                case IFE:
                  return  eval_exp(e->info.node.arguments->elem,tbl,mem)?
                          eval_exp(e->info.node.arguments->next->elem,tbl,mem):
                          eval_exp(e->info.node.arguments->next->next->elem,tbl,mem); 
	        case READINT:
	          printf("> ");
	          scanf("%d", &ret);
	          return ret;
	          break;
	        case READSECRETINT:
	          printf("# ");
	          scanf("%d", &ret);
              newvar(e->info.varname, tbl, TAINTED);
	          return ret;
	          break;
	        default:
	          assert(0); // Unknown/unhandled op.
	        }
        }
    }
}

state_t* eval_stmts(ast_t *p, state_t *state)
{
    ast_list_t *stmts;
    ast_list_t *ip;
    ast_t *t1, *t2;
    ast_t *s;
    value_t v;

    assert(p != NULL);
    assert(p->info.node.tag == SEQ);
    ip = p->info.node.arguments;
    while(ip != NULL) {
	    s = ip->elem;

	    switch(s->info.node.tag){
	        case ASSIGN:
	            /* the lhs */
	            t1 = s->info.node.arguments->elem;
	            /* the rhs */
	            t2 = s->info.node.arguments->next->elem;
	            v = eval_exp(t2, state->tbl, state->mem);
	            switch(t1->tag){
	                case var_ast:
	        	        state->tbl = update_var(t1->info.string, v, state->tbl);
	        	        break;
	                case node_ast:
	        	        assert(t1->info.node.tag == MEM);
	        	        state->mem = store(eval_exp(t1->info.node.arguments->elem,
                              state->tbl, state->mem), v, state->mem);
	        	        break;
	                default:
	        	        assert(0);
	            }
	            break;
	        case PRINT:
	            switch(s->info.node.arguments->elem->tag){
	                case str_ast:
	        	        printf("%s\n", s->info.node.arguments->elem->info.string);
	        	        break;
	                default:
	        	        printf("%u\n", eval_exp(s->info.node.arguments->elem, 
                              state->tbl, state->mem));
	        	        break;
	            }
	            break;
	        case IF:
                if(eval_exp(s->info.node.arguments->elem, state->tbl, state->mem)){
	        	    state = eval_stmts(s->info.node.arguments->next->elem, state);
	            } else {
	        	    state = eval_stmts(s->info.node.arguments->next->next->elem, state);
                } 
	            break;
	        case SEQ:
	            state = eval_stmts(s->info.node.arguments->next->elem, state);
	            break;
	        case ASSERT:
	            if(eval_exp(s->info.node.arguments->elem, state->tbl,state->mem) ==0){
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
