#if !defined(LET_H)
#define LET_H

#include <stdio.h>

struct D_Array;
struct D_List;
struct D_Set;
struct E_Let;
struct Evaluator;

struct E_Let* let_new(struct D_List* bindings, int nBindings);
void let_free(struct E_Let* self);

struct E_Let* let_deepCopy(struct E_Let* self);
void let_eval(struct E_Let* self, struct Evaluator* etor);
void let_freeVars(struct E_Let* self, struct D_Set* freeVars, struct Evaluator* etor);
void let_markChildren(struct E_Let* self);
void let_show(struct E_Let* self, FILE* fp);
size_t let_sizeOf(struct E_Let* self);
size_t let_structSize(void);

#endif
