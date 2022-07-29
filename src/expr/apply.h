#if !defined(APPLY_H)
#define APPLY_H

#include <stdio.h>

struct Any;
struct D_List;
struct D_Set;
struct E_Apply;
struct Evaluator;

struct E_Apply* apply_new(struct Any* abstr, struct D_List* args);
void apply_free(struct E_Apply* self);

struct E_Apply* apply_deepCopy(struct E_Apply* self);
void apply_eval(struct E_Apply* self, struct Evaluator* etor);
void apply_freeVars(struct E_Apply* self, struct D_Set* freeVars, struct Evaluator* etor);
void apply_markChildren(struct E_Apply* self);
void apply_show(struct E_Apply* self, FILE* fp);
size_t apply_sizeOf(struct E_Apply* self);
size_t apply_structSize();

#endif
