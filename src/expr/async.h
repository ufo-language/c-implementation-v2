#if !defined(ASYNC_H)
#define ASYNC_H

#include <stdio.h>

struct D_Array;
struct D_Set;
struct E_Async;
struct Evaluator;

struct E_Async* async_new(struct D_Array* exprs);
void async_free(struct E_Async* self);

struct E_Async* async_deepCopy(struct E_Async* self);
void async_eval(struct E_Async* self, struct Evaluator* etor);
void async_freeVars(struct E_Async* self, struct D_Set* freeVars, struct Evaluator* etor);
void async_markChildren(struct E_Async* self);
void async_show(struct E_Async* self, FILE* fp);
size_t async_sizeOf(struct E_Async* self);
size_t async_structSize(void);

#endif
