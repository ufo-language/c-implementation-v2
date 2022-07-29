#if !defined(DO_H)
#define DO_H

#include <stdio.h>

struct D_Array;
struct D_Set;
struct E_Do;
struct Evaluator;

struct E_Do* do_new(struct D_Array* exprs);
void do_free(struct E_Do* self);

struct E_Do* do_deepCopy(struct E_Do* self);
void do_eval(struct E_Do* self, struct Evaluator* etor);
void do_freeVars(struct E_Do* self, struct D_Set* freeVars, struct Evaluator* etor);
void do_markChildren(struct E_Do* self);
void do_show(struct E_Do* self, FILE* fp);
size_t do_sizeOf(struct E_Do* self);
size_t do_structSize();

#endif
