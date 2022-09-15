#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/closure.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/apply.h"
#include "expr/continuation.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct E_Apply {
    struct Any obj;
    struct Any* abstr;
    struct D_List* args;
};

struct Methods* apply_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))apply_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))apply_eval;
    methods->m_free = (void (*)(struct Any*))apply_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))apply_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))apply_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))apply_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))apply_sizeOf;
    methods->m_structSize = apply_structSize;
    return methods;
}

struct E_Apply* apply_new(struct Any* abstr, struct D_List* args) {
    struct E_Apply* self = (struct E_Apply*)gc_alloc(T_Apply);
    self->abstr = abstr;
    self->args = args;
    return self;
}

void apply_free(struct E_Apply* self) {
    free(self);
}

static void _closureArgs_contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Closure* closure = (struct D_Closure*)arg;
    struct D_List* args = (struct D_List*)evaluator_popObj(etor);
    closure_apply(closure, args, etor);
}

static void _primitiveArgs_contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Primitive* prim = (struct D_Primitive*)arg;
    struct D_List* args = (struct D_List*)evaluator_popObj(etor);
    primitive_apply(prim, args, etor);
}

// also used by binOp_eval
void apply_contin(struct Evaluator* etor, struct Any* arg) {
    struct D_List* args = (struct D_List*)arg;
    struct Any* abstr = evaluator_popObj(etor);
    enum TypeId abstrType = any_typeId(abstr);
    if (T_Closure == abstrType) {
        struct E_Continuation* contin = continuation_new(_closureArgs_contin, "closureApplyArgs", (struct Any*)abstr);
        evaluator_pushExpr(etor, (struct Any*)contin);
        evaluator_pushExpr(etor, (struct Any*)args);
    }
    else if (T_Primitive == abstrType) {
        struct D_Primitive* prim = (struct D_Primitive*)abstr;
        if (primitive_isMacro(prim)) {
            // call primitive without evaluating the arguments
            primitive_apply(prim, args, etor);
        }
        else {
            // evaluate the arguments before calling the primitive
            struct E_Continuation* contin = continuation_new(_primitiveArgs_contin, "primitiveApplyArgs", (struct Any*)abstr);
            evaluator_pushExpr(etor, (struct Any*)contin);
            evaluator_pushExpr(etor, (struct Any*)args);
        }
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Apply"),
            "object is not applyable",
            abstr
        );
    }
}

struct E_Apply* apply_deepCopy(struct E_Apply* self) {
    return apply_new(any_deepCopy(self->abstr), list_deepCopy(self->args));
}

void apply_eval(struct E_Apply* self, struct Evaluator* etor) {
    struct E_Continuation* contin = continuation_new(apply_contin, "apply", (struct Any*)self->args);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, (struct Any*)self->abstr);
}

void apply_freeVars(struct E_Apply* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->abstr, freeVars, etor);
    any_freeVars((struct Any*)self->args, freeVars, etor);
}

void apply_markChildren(struct E_Apply* self) {
    any_mark(self->abstr);
    any_mark((struct Any*)self->args);
}

void apply_show(struct E_Apply* self, FILE* fp) {
    if (any_isA(self->abstr, T_Abstraction) || any_isA(self->abstr, T_Closure)) {
        fputc('(', fp);
        any_show(self->abstr, fp);
        fputc(')', fp);
    }
    else {
        any_show(self->abstr, fp);
    }
    list_showWith(self->args, "(", ", ", ")", fp);
}

size_t apply_sizeOf(struct E_Apply* self) {
    return sizeof(self);
}

size_t apply_structSize(void) {
    return sizeof(struct E_Apply);
}
