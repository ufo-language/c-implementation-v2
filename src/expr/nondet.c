#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/continuation.h"
#include "expr/nondet.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct E_Nondet {
    struct Any obj;
    struct D_Array* exprs;
};

struct Methods* nondet_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))nondet_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))nondet_eval;
    methods->m_free = (void (*)(struct Any*))nondet_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))nondet_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))nondet_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))nondet_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))nondet_sizeOf;
    methods->m_structSize = nondet_structSize;
    return methods;
}

struct E_Nondet* nondet_new(struct D_Array* exprs) {
    struct E_Nondet* self = (struct E_Nondet*)gc_alloc(T_Nondet);
    self->exprs = exprs;
    return self;
}

void nondet_free(struct E_Nondet* self) {
    free(self);
}

struct E_Nondet* nondet_deepCopy(struct E_Nondet* self) {
    return nondet_new(array_deepCopy(self->exprs));
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Array* argAry = (struct D_Array*)arg;
    int nThreads = array_count(argAry);
    struct Any* value = NULL;
    // find the first terminated thread
    int n = 0;
    for (; n<nThreads; n++) {
        struct Evaluator* thread = (struct Evaluator*)array_get_unsafe(argAry, n);
        if (TS_Terminated == evaluator_getThreadStatus(thread)) {
            value = evaluator_popObj(thread);
            break;
        }
    }
    // terminate the remaining threads
    for (; n<nThreads; n++) {
        struct Evaluator* thread = (struct Evaluator*)array_get_unsafe(argAry, n);
        if (TS_Terminated != evaluator_getTid(thread)) {
            threadManager_terminateThread(thread);
        }
    }
    evaluator_pushObj(etor, value);
}

void nondet_eval(struct E_Nondet* self, struct Evaluator* etor) {
    struct D_Array* exprAry = self->exprs;
    int nExprs = array_count(exprAry);
    if (nExprs == 0) {
        evaluator_pushObj(etor, (struct Any*)NIL);
        return;
    }
    struct D_Array* threadAry = array_new(nExprs);
    // spawn each thread
    struct D_Triple* currentEnv = evaluator_getEnv(etor);
    for (int n=0; n<nExprs; n++) {
        struct Any* expr = array_get_unsafe(exprAry, n);
        struct Evaluator* thread = evaluator_new();
        evaluator_setEnv(thread, currentEnv);
        evaluator_pushExpr(thread, (struct Any*)expr);
        threadManager_addThread(thread);
        array_set_unsafe(threadAry, n, (struct Any*)thread);
        threadManager_blockThread(etor, (struct Any*)thread);
    }
    struct E_Continuation* contin = continuation_new(_contin, "nondet", (struct Any*)threadAry);
    evaluator_pushExpr(etor, (struct Any*)contin);
}

void nondet_freeVars(struct E_Nondet* self, struct D_Set* freeVars, struct Evaluator* etor) {
    array_freeVars(self->exprs, freeVars, etor);
}

void nondet_markChildren(struct E_Nondet* self) {
    any_mark((struct Any*)self->exprs);
}

void nondet_show(struct E_Nondet* self, FILE* fp) {
    array_showWith(self->exprs, "nondet ", " ", " end", fp);
}

size_t nondet_sizeOf(struct E_Nondet* self) {
    return sizeof(self);
}

size_t nondet_structSize(void) {
    return sizeof(struct E_Nondet);
}
