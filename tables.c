#include <stdlib.h>
#include <string.h>
#include "tables.h"

#define dbg_printf(...) \
   do { if (eval_debug) fprintf(stderr, __VA_ARGS__); } while (0)

extern int eval_debug;

/**
 * Prints list of tainted items to stderr
 */
void print_tainted_items(exp_info *ei) {
    tainted_item *t;
    t = ei->ti;
    if (t == NULL) {
        fprintf(stderr, "Direct");
    }

    while (t != NULL) {
        fprintf(stderr, "%s", t->name);
        if (t->next != NULL) {
            fprintf(stderr, ",");
        }
        t = t->next;
    }
}

/**
 * Add to tainted item list.
 */
void add_tainted_var(exp_info *ei, varctx_t *c) {
    // TODO Probably free this memory someday.
    if (ei->ti == NULL) {
        // This is the first tainted item
        ei->ti = (tainted_item *) malloc(sizeof(tainted_item));
        strncpy(ei->ti->name, c->name, 1024);
        ei->ti->next = NULL;
    } else {
        tainted_item *t = ei->ti;
        while (t->next != NULL) {
            t = t->next;
        }
        t->next = (tainted_item *) malloc(sizeof(tainted_item));
        strncpy(t->next->name, c->name, 1024);
        t->next->next = NULL;
    }
}

/**
 * TODO
 */
//void add_tainted_mem();

/**
 * Add the NEWVAR to the end of the context.
 *
 * Returns NEWVAR, which should be used if context
 * is NULL (in which case NEWVAR obviously won't be
 * added to the end of a NULL pointer).
 */
varctx_t *newvar(char *name, varctx_t *o, int tainted) {
    varctx_t *n = (varctx_t *)malloc(sizeof(varctx_t));
    dbg_printf("[Debug][newvar] Creating new [%s] var: %s\n",
                tainted == 1 ? "TAINTED" : "safe",
                name);
    n->name = name;
    n->val = DEFAULT_VAL;
    n->tainted = tainted;
    n->next = NULL;
    if (o != NULL) {
        while (o->next != NULL) {
            o = o->next;
        }
        o->next = n;
    }
    return n;
}

value_t lookup_var(char *name, varctx_t *c, exp_info *ei) {
    dbg_printf("[Debug][lookup_var] For name %s\n", name);
    while(c != NULL){
        if(strcmp(c->name, name) == 0) {
            dbg_printf("[Debug][lookup_var]: %s value: %x. Tainted: %s\n", name,
                        c->val, c->tainted == 1 ? "YES" : "NO" );
            ei->tainted |= c->tainted;
            if (c->tainted == 1) {
                add_tainted_var(ei, c);
            }
            return c->val;
        }
        c=c->next;
    }
    dbg_printf("[Debug][lookup_var]: %s <uninitialized. returning %d>", name, DEFAULT_VAL);
    ei->tainted = 0;
    return DEFAULT_VAL;
}

varctx_t * update_var(char *name, value_t val, varctx_t *o, int tainted) {
    dbg_printf("[Debug][update_var] Called for %s\n", name);
    varctx_t *c = o;
    varctx_t *n = NULL;
    while (c != NULL) {
        dbg_printf("[Debug][update_var] Observing %p\n", c);
        if (strcmp(c->name, name) == 0) {
	        dbg_printf("[Debug][update_var] %s with %x (old value %x)\n", name, val, c->val);
            c->val = val;
            c->tainted = tainted;
            return o;
        }
        dbg_printf("[Debug][update_var] %s did not match. \n", c->name);
        c = c->next;
        dbg_printf("[Debug][update_var] c updated to %p\n", c);
    }
    n = newvar(name, o, tainted);
    n->val = val;
    dbg_printf("[Debug][update_var] %s with %x (new node)\n", name, val);
    return n;
}

memctx_t *store(unsigned int addr, value_t val, memctx_t *o) {
    memctx_t *n = NULL;
    memctx_t *c = o;
    while(c != NULL){
        if(c->addr == addr){
            printf("[Debug][store] %x with %x (replacing %x)\n", c->addr,
                  val, c->val);
            c->val = val;
            return o;
        }
        c = c->next;
    }
    /* we didn't find the address. create a new spot in the context */
    n = (memctx_t *)(malloc(sizeof(memctx_t)));
    n->addr = addr;
    n->val = val;
    n->next = o;
    dbg_printf("[Debug][store] %x with %x (new node)\n", n->addr, val);
    return n;
}

value_t load(unsigned int addr, memctx_t *c)
{
  while(c != NULL){
    if(c->addr == addr){
	  dbg_printf("[Debug][load]: %x value: %x\n", addr, c->val);
      return c->val;
    }
    c = c->next;
  }
  dbg_printf("[Debug][load]: %x <uninitialized. returning %x>\n", addr, DEFAULT_VAL);
  return DEFAULT_VAL;
}


void print_memctx(memctx_t *c)
{
  while(c != NULL){
    printf("[Debug] mem[%x] =  %x\n", c->addr, c->val);
    c = c->next;
  }
}
