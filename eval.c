#include "ast.h"
#include "tables.h"
#include "eval.h"
#include <assert.h>

#define dbg_printf(...) \
    do { if (eval_debug) fprintf(stderr, __VA_ARGS__); } while (0)

int eval_debug = 0;

void debug_eval(int val)
{
    eval_debug = val;
}

value_t eval_exp(ast_t *e, varctx_t *tbl, memctx_t *mem, exp_info *ei, int is_mem)
{
    value_t ret;
    switch(e->tag){
        case int_ast:
	        dbg_printf("[Debug][eval_exp][int_ast]\n");
            return e->info.integer;
            break;
        case var_ast:
            dbg_printf("[Debug][eval_exp][var_ast][varname: %s]\n", e->info.varname);
            return lookup_var(e->info.varname, tbl,ei,is_mem);
            break;
        case node_ast: {
	        dbg_printf("[Debug][eval_exp][node_ast]\n");
        switch(e->info.node.tag){
	        case MEM:
	            dbg_printf("[Debug][eval_exp][MEM load]\n");
                int addr = eval_exp(e->info.node.arguments->elem,tbl,mem,ei,IS_MEM);
                ret = load(addr, mem);
                check_tainted_list_mem(ei, addr);
	            return ret;   
	            break;
	        case PLUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) + 
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM);
	          break;
	        case MINUS:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) -
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM);
	          break;
	        case DIVIDE:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) /
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM);
	          break;
	        case TIMES:
	          return 
	            eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) *
	            eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM);
	          break;

	        case EQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) == 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	          break;
	        case NEQ:
	          return 
	            (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) != 
	             eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	          break;
	        case GT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) > 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	            break;
	        case LT:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) <
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	            break;
	        case LEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) <= 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	            break;
	        case GEQ:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) >=
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	            break;
	        case AND:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) && 
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	            break;
	        case OR:
	          return (eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM) ||
	        	  eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM));
	          break;
	        case NEGATIVE:
	          return -(eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM));
	        case NOT:
	          return !(eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM));
            case IFE:
              return  eval_exp(e->info.node.arguments->elem,tbl,mem,ei,NOT_MEM)?
                      eval_exp(e->info.node.arguments->next->elem,tbl,mem,ei,NOT_MEM):
                      eval_exp(e->info.node.arguments->next->next->elem,tbl,mem,ei,NOT_MEM);
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
    
    // the tainted container for expression
    exp_info ei;
    ei.tainted = 0;
    ei.ti = NULL;

    // the tainted container for memory address
    exp_info ei_addr;
    ei_addr.tainted = 0;
    ei_addr.ti = NULL;
    unsigned int address; 

    if (state->tbl == NULL) {
        // If we have no initial, context, create one.
        state->tbl = newvar("INITIAL_CONTEXT", NULL, 0);
    }

    assert(p != NULL);
    assert(p->info.node.tag == SEQ);
    ip = p->info.node.arguments;
    dbg_printf("[Debug] before while\n");
    while(ip != NULL) {
	    s = ip->elem;
        ei.tainted = 0;
        ei.ti = NULL;
        
        ei_addr.tainted = 0;
        ei_addr.ti = NULL;

	    switch(s->info.node.tag){
            
	        case ASSIGN:
                /* the lhs */
	            t1 = s->info.node.arguments->elem;
	            /* the rhs */
	            t2 = s->info.node.arguments->next->elem;
	            v = eval_exp(t2, state->tbl, state->mem, &ei, NOT_MEM);
	            switch(t1->tag){
	                case var_ast:
                        dbg_printf("[Debug] ASSIGNING to %s\n", t1->info.string);
                        dbg_printf("[Debug][ASSIGN][VAR AST] RHS Tainted: %s\n",
                                (ei.tainted == TAINTED) ? "YES" : "NO");
                        update_var(t1->info.string, v, state->tbl, ei.tainted);
	        	        break;
	                case node_ast:
                        dbg_printf("[Debug] ASSIGNING to MEM[]\n");
	                    dbg_printf("[Debug][ASSIGN][NODE AST]\n");
	        	        assert(t1->info.node.tag == MEM);
	        	        address = eval_exp(t1->info.node.arguments->elem, state->tbl, state->mem, &ei_addr, NOT_MEM); // FIXME FISHY
	                    dbg_printf("[Debug][ASSIGN][NODE AST] Address Tainted: %s\n", (ei_addr.tainted == TAINTED) ? "YES" : "NO");
	                    dbg_printf("[Debug][ASSIGN][NODE AST] Content Tainted: %s\n", (ei.tainted == TAINTED) ? "YES" : "NO");
                        add_remove_tainted_mem(&ei, address);
                        state->mem = store(address, v, state->mem);
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
                                     &ei, NOT_MEM);
                        if (ei.tainted == TAINTED) {
                            // Extract all tainted variables (and memory locations)
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
                if(eval_exp(s->info.node.arguments->elem, state->tbl, state->mem, &ei, NOT_MEM)){
	        	    state = eval_stmts(s->info.node.arguments->next->elem, state);
	            } else {
	        	    state = eval_stmts(s->info.node.arguments->next->next->elem, state);
                } 
	            break;
	        case SEQ:
	            state = eval_stmts(s->info.node.arguments->next->elem, state);
	            break;
	        case ASSERT:
	            if(eval_exp(s->info.node.arguments->elem, state->tbl,state->mem, &ei, NOT_MEM) ==0){
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
