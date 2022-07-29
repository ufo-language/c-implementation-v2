#if !defined(IF_H)
#define IF_H

#include <stdio.h>

struct Any;
struct D_Set;
struct E_If;
struct Evaluator;
struct Evaluator;

struct E_If* if_new(struct Any* cond, struct Any* conseq, struct Any* alt);
void if_free(struct E_If* self);

struct E_If* if_deepCopy(struct E_If* self);
void if_eval(struct E_If* self, struct Evaluator* etor);
void if_freeVars(struct E_If* self, struct D_Set* freeVars, struct Evaluator* etor);
void if_markChildren(struct E_If* self);
void if_show(struct E_If* self, FILE* fp);
size_t if_sizeOf(struct E_If* self);
size_t if_structSize();

#endif

