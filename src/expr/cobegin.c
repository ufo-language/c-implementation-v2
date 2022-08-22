#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/cobegin.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_Cobegin {
    struct Any obj;
    struct D_Array* exprs;
};

struct E_Cobegin* cobegin_new(struct D_Array* exprs) {
    struct E_Cobegin* self = (struct E_Cobegin*)gc_alloc(T_Cobegin);
    self->exprs = exprs;
    return self;
}

void cobegin_free(struct E_Cobegin* self) {
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

struct E_Cobegin* cobegin_deepCopy(struct E_Cobegin* self) {
    return cobegin_new(array_deepCopy(self->exprs));
}

void cobegin_eval(struct E_Cobegin* self, struct Evaluator* etor) {
    printf("%s is incomplete\n", __func__);
    int nExprs = array_count(self->exprs);
    if (nExprs == 0) {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
    else {
        struct D_Integer* arg = integer_new(nExprs - 1);
        struct E_Continuation* contin = continuation_new(_contin, "cobegin", (struct Any*)arg);
        evaluator_pushExprEnv(etor, (struct Any*)contin, (struct Any*)NIL);
        for (int n=nExprs - 1; n>=0; n--) {
            evaluator_pushExprEnv(etor, array_get_unsafe(self->exprs, n), (struct Any*)NIL);
        }
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
