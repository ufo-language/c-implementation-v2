#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/continuation.h"
#include "expr/async.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct E_Async {
    struct Any obj;
    struct D_Array* exprs;
};

struct Methods* async_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))async_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))async_eval;
    methods->m_free = (void (*)(struct Any*))async_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))async_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))async_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))async_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))async_sizeOf;
    methods->m_structSize = async_structSize;
    return methods;
}

struct E_Async* async_new(struct D_Array* exprs) {
    struct E_Async* self = (struct E_Async*)gc_alloc(T_Async);
    self->exprs = exprs;
    return self;
}

void async_free(struct E_Async* self) {
    free(self);
}

struct E_Async* async_deepCopy(struct E_Async* self) {
    return async_new(array_deepCopy(self->exprs));
}

void async_eval(struct E_Async* self, struct Evaluator* etor) {
    struct D_Array* exprAry = self->exprs;
    int nExprs = array_count(exprAry);
    struct D_Array* threadAry = array_new(nExprs);
    struct D_Triple* currentEnv = evaluator_getEnv(etor);
    for (int n=0; n<nExprs; n++) {
        struct Any* expr = array_get_unsafe(exprAry, n);
        struct Evaluator* thread = evaluator_new();
        evaluator_setEnv(thread, currentEnv);
        evaluator_pushExpr(thread, (struct Any*)expr);
        threadManager_addThread(thread);
        array_set_unsafe(threadAry, n, (struct Any*)thread);
    }
    evaluator_pushExpr(etor, (struct Any*)threadAry);
}

void async_freeVars(struct E_Async* self, struct D_Set* freeVars, struct Evaluator* etor) {
    array_freeVars(self->exprs, freeVars, etor);
}

void async_markChildren(struct E_Async* self) {
    any_mark((struct Any*)self->exprs);
}

void async_show(struct E_Async* self, FILE* fp) {
    array_showWith(self->exprs, "async ", " ", " end", fp);
}

size_t async_sizeOf(struct E_Async* self) {
    return sizeof(self);
}

size_t async_structSize(void) {
    return sizeof(struct E_Async);
}
