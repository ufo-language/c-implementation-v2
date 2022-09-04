#if !defined(NONDET_H)
#define NONDET_H

#include <stdio.h>

struct D_Array;
struct D_Set;
struct E_Nondet;
struct Evaluator;

struct E_Nondet* nondet_new(struct D_Array* exprs);
void nondet_free(struct E_Nondet* self);

struct E_Nondet* nondet_deepCopy(struct E_Nondet* self);
void nondet_eval(struct E_Nondet* self, struct Evaluator* etor);
void nondet_freeVars(struct E_Nondet* self, struct D_Set* freeVars, struct Evaluator* etor);
void nondet_markChildren(struct E_Nondet* self);
void nondet_show(struct E_Nondet* self, FILE* fp);
size_t nondet_sizeOf(struct E_Nondet* self);
size_t nondet_structSize(void);

#endif
