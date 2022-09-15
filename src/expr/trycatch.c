#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "expr/exnhandler.h"
#include "expr/trycatch.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct E_TryCatch {
    struct Any obj;
    struct Any* tryExpr;
    struct E_Abstraction* catchExpr;
};

struct Methods* tryCatch_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))tryCatch_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))tryCatch_eval;
    methods->m_free = (void (*)(struct Any*))tryCatch_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))tryCatch_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))tryCatch_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))tryCatch_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))tryCatch_sizeOf;
    methods->m_structSize = tryCatch_structSize;
    return methods;
}

struct E_TryCatch* tryCatch_new(struct Any* tryExpr, struct E_Abstraction* catchExpr) {
    struct E_TryCatch* self = (struct E_TryCatch*)gc_alloc(T_TryCatch);
    self->tryExpr = tryExpr;
    self->catchExpr = catchExpr;
    return self;
}

void tryCatch_free(struct E_TryCatch* self) {
    free(self);
}

struct E_TryCatch* tryCatch_deepCopy(struct E_TryCatch* self) {
    return tryCatch_new(any_deepCopy(self->tryExpr), abstraction_deepCopy(self->catchExpr));
}

void tryCatch_eval(struct E_TryCatch* self, struct Evaluator* etor) {
    struct D_List* ostack = evaluator_getOStack(etor);
    struct D_Triple* env = evaluator_getEnv(etor);
    struct E_ExceptionHandler* exnHandler = exceptionHandler_new(self->catchExpr, ostack, env);
    evaluator_pushExpr(etor, (struct Any*)exnHandler);
    evaluator_pushExpr(etor, self->tryExpr);
}

void tryCatch_freeVars(struct E_TryCatch* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->tryExpr, freeVars, etor);
    any_freeVars((struct Any*)self->catchExpr, freeVars, etor);
}

void tryCatch_markChildren(struct E_TryCatch* self) {
    any_mark((struct Any*)self->tryExpr);
    any_mark((struct Any*)self->catchExpr);
}

void tryCatch_show(struct E_TryCatch* self, FILE* fp) {
    fputs("try ", fp);
    any_show(self->tryExpr, fp);
    fputs(" catch ", fp);
    any_show((struct Any*)self->catchExpr, fp);
}

size_t tryCatch_sizeOf(struct E_TryCatch* self) {
    return sizeof(self);
}

size_t tryCatch_structSize(void) {
    return sizeof(struct E_TryCatch);
}
