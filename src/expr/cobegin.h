#if !defined(COBEGIN_H)
#define COBEGIN_H

#include <stdio.h>

struct D_Array;
struct D_Set;
struct E_Cobegin;
struct Evaluator;

struct E_Cobegin* cobegin_new(struct D_Array* exprs);
void cobegin_free(struct E_Cobegin* self);

struct E_Cobegin* cobegin_deepCopy(struct E_Cobegin* self);
void cobegin_eval(struct E_Cobegin* self, struct Evaluator* etor);
void cobegin_freeVars(struct E_Cobegin* self, struct D_Set* freeVars, struct Evaluator* etor);
void cobegin_markChildren(struct E_Cobegin* self);
void cobegin_show(struct E_Cobegin* self, FILE* fp);
size_t cobegin_sizeOf(struct E_Cobegin* self);
size_t cobegin_structSize();

#endif
