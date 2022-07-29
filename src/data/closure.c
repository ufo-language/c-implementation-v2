#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/closure.h"
#include "data/list.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "gc/gc.h"

struct D_Closure {
    struct Any obj;
    struct E_Abstraction* abstr;
    struct D_Triple* env;
};

struct D_Closure* closure_new(struct E_Abstraction* abstr, struct D_Triple* env) {
    struct D_Closure* self = (struct D_Closure*)gc_alloc(T_Closure);
    self->abstr = abstr;
    self->env = env;
    return self;
}

void closure_free(struct D_Closure* self) {
    free(self);
}

void closure_apply(struct D_Closure* self, struct D_List* args, struct Evaluator* etor) {
    struct E_Abstraction* abstr = self->abstr;
    struct D_Triple* env = self->env;
    while (abstr != NULL) {
        struct D_List* params = abstraction_getParams(abstr);
        struct D_Triple* env1 = any_match((struct Any*)params, (struct Any*)args, env);
        if (env1 != NULL) {
            evaluator_saveEnv(etor);
            evaluator_pushExprEnv(etor, (struct Any*)abstraction_getBody(abstr), (struct Any*)env1);
            return;
        }
        abstr = abstraction_getNext(abstr);
    }
    evaluator_throwException(
        etor,
        any_typeSymbol((struct Any*)self),
        "no matching rule in function",
        (struct Any*)array_newN(2, (struct Any*)self, (struct Any*)args)
    );
}

void closure_freeVars(struct D_Closure* self, struct D_Set* freeVars, struct Evaluator* etor) {
    abstraction_freeVars(self->abstr, freeVars, etor);
}

struct E_Abstraction* closure_getAbstr(struct D_Closure* self) {
    return self->abstr;
}

struct D_Triple* closure_getEnv(struct D_Closure* self) {
    return self->env;
}

void closure_markChildren(struct D_Closure* self) {
    any_mark((struct Any*)self->abstr);
    any_mark((struct Any*)self->env);
}

void closure_show(struct D_Closure* self, FILE* fp) {
    abstraction_show((struct E_Abstraction*)self->abstr, fp);
}

size_t closure_sizeOf(struct D_Closure* self) {
    (void)self;
    return sizeof(struct D_Closure);
}

size_t closure_structSize() {
    return sizeof(struct D_Closure);
}
