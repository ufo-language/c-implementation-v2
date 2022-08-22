#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/set.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "utils/hash.h"

struct E_ExceptionHandler {
    struct Any obj;
    struct E_Abstraction* catchExpr;
    struct D_List* ostack;
    struct D_Triple* env;
};

struct E_ExceptionHandler* exceptionHandler_new(struct E_Abstraction* catchExpr, struct D_List* ostack, struct D_Triple* env) {
    struct E_ExceptionHandler* self = (struct E_ExceptionHandler*)gc_alloc(T_ExceptionHandler);
    self->catchExpr = catchExpr;
    self->ostack = ostack;
    self->env = env;
    return self;
}

void exceptionHandler_free(struct E_ExceptionHandler* self) {
    free(self);
}

bool exceptionHandler_apply(struct E_ExceptionHandler* self, struct Any* arg, struct Evaluator* etor) {
    struct D_List* args = list_new(arg, (struct Any*)EMPTY_LIST);
    struct E_Abstraction* abstr = self->catchExpr;
    struct D_Triple* env = EMPTY_TRIPLE;
    while (abstr != NULL) {
        struct D_List* params = abstraction_getParams(abstr);
        struct D_Triple* env1 = any_match((struct Any*)params, (struct Any*)args, env);
        if (env1 != NULL) {
            evaluator_saveEnv(etor);
            evaluator_pushExprEnv(etor, (struct Any*)abstraction_getBody(abstr), (struct Any*)env1);
            evaluator_clearException(etor);
            return true;
        }
        abstr = abstraction_getNext(abstr);
    }
    return false;
}

void exceptionHandler_eval(struct E_ExceptionHandler* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
    // By default an exception handler does nothing.
    // It's not 'invoked' until an exception occurs that matches the handler.
    // DO NOTHING.
}

void exceptionHandler_markChildren(struct E_ExceptionHandler* self) {
    any_mark((struct Any*)self->catchExpr);
    any_mark((struct Any*)self->ostack);
    any_mark((struct Any*)self->env);
}

void exceptionHandler_show(struct E_ExceptionHandler* self, FILE* fd) {
    any_show((struct Any*)self->catchExpr, fd);
}

size_t exceptionHandler_sizeOf(struct E_ExceptionHandler* self) {
    return sizeof(self);
}

size_t exceptionHandler_structSize(void) {
    return sizeof(struct E_ExceptionHandler);
}
