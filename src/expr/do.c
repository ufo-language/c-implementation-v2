#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/do.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct E_Do {
    struct Any obj;
    struct D_Array* exprs;
};

struct Methods* do_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))do_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))do_eval;
    methods->m_free = (void (*)(struct Any*))do_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))do_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))do_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))do_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))do_sizeOf;
    methods->m_structSize = do_structSize;
    return methods;
}

struct E_Do* do_new(struct D_Array* exprs) {
    struct E_Do* self = (struct E_Do*)gc_alloc(T_Do);
    self->exprs = exprs;
    return self;
}

void do_free(struct E_Do* self) {
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

struct E_Do* do_deepCopy(struct E_Do* self) {
    return do_new(array_deepCopy(self->exprs));
}

void do_eval(struct E_Do* self, struct Evaluator* etor) {
    int nExprs = array_count(self->exprs);
    if (nExprs == 0) {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
    else {
        struct D_Integer* arg = integer_new(nExprs == 0 ? 0 : nExprs - 1);
        struct E_Continuation* contin = continuation_new(_contin, "do", (struct Any*)arg);
        evaluator_pushExprEnv(etor, (struct Any*)contin, (struct Any*)NIL);
        for (int n=nExprs - 1; n>=0; n--) {
            evaluator_pushExprEnv(etor, array_get_unsafe(self->exprs, n), (struct Any*)NIL);
        }
    }
}

void do_freeVars(struct E_Do* self, struct D_Set* freeVars, struct Evaluator* etor) {
    array_freeVars(self->exprs, freeVars, etor);
}

void do_markChildren(struct E_Do* self) {
    any_mark((struct Any*)self->exprs);
}

void do_show(struct E_Do* self, FILE* fp) {
    array_showWith(self->exprs, "do ", " ", " end", fp);
}

size_t do_sizeOf(struct E_Do* self) {
    return sizeof(self);
}

size_t do_structSize(void) {
    return sizeof(struct E_Do);
}
