#if !defined(LOOP_H)
#define LOOP_H

#include <stdio.h>

struct D_Array;
struct D_List;
struct D_Set;
struct E_Loop;
struct Evaluator;

struct E_Loop* loop_new(struct Any* iterExpr, struct Any* body);
void loop_free(struct E_Loop* self);

struct E_Loop* loop_deepCopy(struct E_Loop* self);
void loop_eval(struct E_Loop* self, struct Evaluator* etor);
void loop_freeVars(struct E_Loop* self, struct D_Set* freeVars, struct Evaluator* etor);
void loop_markChildren(struct E_Loop* self);
void loop_show(struct E_Loop* self, FILE* fp);
size_t loop_sizeOf(struct E_Loop* self);
size_t loop_structSize(void);

#endif
