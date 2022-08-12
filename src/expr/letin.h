#if !defined(LETIN_H)
#define LETIN_H

#include <stdio.h>

struct Any;
struct D_Array;
struct D_List;
struct D_Set;
struct E_LetIn;
struct Evaluator;

struct E_LetIn* letIn_new(struct D_List* bindings, int nBindings, struct Any* expr);
void letIn_free(struct E_LetIn* self);

struct E_LetIn* letIn_deepCopy(struct E_LetIn* self);
void letIn_eval(struct E_LetIn* self, struct Evaluator* etor);
void letIn_freeVars(struct E_LetIn* self, struct D_Set* freeVars, struct Evaluator* etor);
void letIn_markChildren(struct E_LetIn* self);
void letIn_show(struct E_LetIn* self, FILE* fp);
size_t letIn_sizeOf(struct E_LetIn* self);
size_t letIn_structSize(void);

#endif
