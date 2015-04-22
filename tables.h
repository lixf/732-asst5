#ifndef taint_hw_tables_h
#define taint_hw_tables_h

#include <stdio.h>

typedef int value_t;

#define DEFAULT_VAL 0

typedef struct varctx_t {
  char *name;
  value_t val;
  struct varctx_t *next;
  int tainted; 
} varctx_t;

typedef struct memctx_t {
  unsigned int addr;
  value_t val;
  struct memctx_t *next;
} memctx_t; 

typedef struct tainted_item {
    char name[1024];
    struct tainted_item *next;
} tainted_item;

typedef struct exp_info {
    int tainted;    /* Is this entire expression tainted? */
    tainted_item *ti;
} exp_info;


void print_tainted_items(exp_info *ei);

/* Extends the context o to include a new variable. The initial value
   is DEFAULT_VAL */
varctx_t *newvar(char *name, varctx_t *o, int tainted);

/* returns the value corresponding to a variable in a context. Returns
   DEFAULT_VAL if no such name exists */
value_t lookup_var(char *name, varctx_t *c, exp_info *ei, int is_mem);

/* update a variable. returns a new context, which may be different
   than c */
varctx_t *update_var(char *name, value_t val, varctx_t *c, int tainted);

/* updates our context c to bind value to address. Returns a new
   context with the updated binding (which may be different than c) */
memctx_t *store(unsigned int addr, value_t val, memctx_t *c);

/* load a value. Returns DEFAULT_VAL if there is no value for addr */
value_t load(unsigned int addr, memctx_t *c);

/* prints out the memory context */
void print_memctx(memctx_t *);

#endif
