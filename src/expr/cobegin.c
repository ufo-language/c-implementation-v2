#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/continuation.h"
#include "expr/cobegin.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct E_Cobegin {
    struct Any obj;
    struct D_Array* exprs;
};

struct Methods* cobegin_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))cobegin_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))cobegin_eval;
    methods->m_free = (void (*)(struct Any*))cobegin_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))cobegin_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))cobegin_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))cobegin_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))cobegin_sizeOf;
    methods->m_structSize = cobegin_structSize;
    return methods;
}

struct E_Cobegin* cobegin_new(struct D_Array* exprs) {
    struct E_Cobegin* self = (struct E_Cobegin*)gc_alloc(T_Cobegin);
    self->exprs = exprs;
    return self;
}

void cobegin_free(struct E_Cobegin* self) {
    free(self);
}

struct E_Cobegin* cobegin_deepCopy(struct E_Cobegin* self) {
    return cobegin_new(array_deepCopy(self->exprs));
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Array* argAry = (struct D_Array*)arg;
    struct D_Integer* threadIndexInt = (struct D_Integer*)array_get_unsafe(argAry, 0);
    int threadIndex = integer_getValue(threadIndexInt);
    struct D_Array* threadAry = (struct D_Array*)array_get_unsafe(argAry, 1);
    struct Evaluator* thread = (struct Evaluator*)array_get_unsafe(threadAry, threadIndex);
    struct Any* threadValue = evaluator_popObj(thread);
    struct D_Array* resultAry = (struct D_Array*)array_get_unsafe(argAry, 2);
    array_set_unsafe(resultAry, threadIndex, threadValue);
    if (threadIndex == 0) {
        // finished waiting for all threads
        evaluator_pushObj(etor, (struct Any*)resultAry);
    }
    else {
        threadIndex--;
        array_set_unsafe(argAry, 0, (struct Any*)integer_new(threadIndex));
        struct E_Continuation* contin = continuation_new(_contin, "cobegin", (struct Any*)argAry);
        evaluator_pushExpr(etor, (struct Any*)contin);
        struct Evaluator* nextThread = (struct Evaluator*)array_get_unsafe(threadAry, threadIndex);
        threadManager_blockThread(etor, (struct Any*)nextThread);
    }
}

void cobegin_eval(struct E_Cobegin* self, struct Evaluator* etor) {
    struct D_Array* exprAry = self->exprs;
    int nExprs = array_count(exprAry);
    struct D_Array* threadAry = array_new(nExprs);
    // spawn each thread
    struct D_Triple* currentEnv = evaluator_getEnv(etor);
    struct Evaluator* thread = NULL;
    for (int n=0; n<nExprs; n++) {
        struct Any* expr = array_get_unsafe(exprAry, n);
        thread = evaluator_new();
        evaluator_setEnv(thread, currentEnv);
        evaluator_pushExpr(thread, (struct Any*)expr);
        threadManager_addThread(thread);
        array_set_unsafe(threadAry, n, (struct Any*)thread);
    }
    if (thread != NULL) {
        // join the last-spawned thread and get its value
        // (the calculation is a little simpler if we work backward through the array of threads)
        struct D_Array* argAry = array_newN(3, integer_new(nExprs - 1), threadAry, array_new(nExprs));
        struct E_Continuation* contin = continuation_new(_contin, "cobegin", (struct Any*)argAry);
        evaluator_pushExpr(etor, (struct Any*)contin);
        threadManager_blockThread(etor, (struct Any*)thread);
    }
    else {
        evaluator_pushObj(etor, (struct Any*)EMPTY_ARRAY);
    }
}

void cobegin_freeVars(struct E_Cobegin* self, struct D_Set* freeVars, struct Evaluator* etor) {
    array_freeVars(self->exprs, freeVars, etor);
}

void cobegin_markChildren(struct E_Cobegin* self) {
    any_mark((struct Any*)self->exprs);
}

void cobegin_show(struct E_Cobegin* self, FILE* fp) {
    array_showWith(self->exprs, "cobegin ", " ", " end", fp);
}

size_t cobegin_sizeOf(struct E_Cobegin* self) {
    return sizeof(self);
}

size_t cobegin_structSize(void) {
    return sizeof(struct E_Cobegin);
}
