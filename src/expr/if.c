#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/list.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/if.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct E_If {
    struct Any obj;
    struct Any* cond;
    struct Any* conseq;
    struct Any* alt;
};

struct Methods* if_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))if_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))if_eval;
    methods->m_free = (void (*)(struct Any*))if_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))if_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))if_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))if_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))if_sizeOf;
    methods->m_structSize = if_structSize;
    return methods;
}

struct E_If* if_new(struct Any* cond, struct Any* conseq, struct Any* alt) {
    struct E_If* self = (struct E_If*)gc_alloc(T_If);
    self->cond = cond;
    self->conseq = conseq;
    self->alt = alt;
    return self;
}

void if_free(struct E_If* self) {
    free(self);
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct Any* conseq = list_getFirst((struct D_List*)arg);
    struct Any* alt = list_getRest((struct D_List*)arg);
    struct Any* condVal = evaluator_popObj(etor);;
    struct Any* resExpr = any_boolValue(condVal) ? conseq : alt;
    evaluator_pushExpr(etor, resExpr);
}

struct E_If* if_deepCopy(struct E_If* self) {
    return if_new(any_deepCopy(self->cond), any_deepCopy(self->conseq), any_deepCopy(self->alt));
}

void if_eval(struct E_If* self, struct Evaluator* etor) {
    struct D_List* arg = list_new(self->conseq, self->alt);
    struct E_Continuation* contin = continuation_new(_contin, "if", (struct Any*)arg);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, (struct Any*)self->cond);
}

void if_freeVars(struct E_If* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->cond, freeVars, etor);
    any_freeVars(self->conseq, freeVars, etor);
    any_freeVars(self->alt, freeVars, etor);
}

void if_markChildren(struct E_If* self) {
    any_mark(self->cond);
    any_mark(self->conseq);
    any_mark(self->alt);
}

void if_show(struct E_If* self, FILE* fp) {
    fputs("if ", fp);
    any_show(self->cond, fp);
    fputs(" then ", fp);
    any_show(self->conseq, fp);
    fputs(" else ", fp);
    any_show(self->alt, fp);
    fputs(" end", fp);
}

size_t if_sizeOf(struct E_If* self) {
    return sizeof(self);
}

size_t if_structSize(void) {
    return sizeof(struct E_If);
}
