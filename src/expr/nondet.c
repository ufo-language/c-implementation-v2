#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
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

static void _contin(struct Evaluator* etor, struct Any* arg) {
    int nDrops = integer_getValue((struct D_Integer*)arg);
    if (nDrops > 0) {
        struct Any* res = evaluator_popObj(etor);
        for (int n=0; n<nDrops; n++) {
            evaluator_popObj(etor);
        }
        evaluator_pushObj(etor, res);
    }
}

struct E_Nondet* nondet_deepCopy(struct E_Nondet* self) {
    return nondet_new(array_deepCopy(self->exprs));
}

void nondet_eval(struct E_Nondet* self, struct Evaluator* etor) {
    int nExprs = array_count(self->exprs);
    if (nExprs == 0) {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
    else {
        struct D_Integer* arg = integer_new(nExprs == 0 ? 0 : nExprs - 1);
        struct E_Continuation* contin = continuation_new(_contin, "nondet", (struct Any*)arg);
        evaluator_pushExprEnv(etor, (struct Any*)contin, (struct Any*)NIL);
        for (int n=nExprs - 1; n>=0; n--) {
            evaluator_pushExprEnv(etor, array_get_unsafe(self->exprs, n), (struct Any*)NIL);
        }
    }
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
