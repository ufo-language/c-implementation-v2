#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/letin.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct E_LetIn {
    struct Any obj;
    int nBindings;
    struct D_List* bindings;
    struct Any* expr;
};

struct Methods* letIn_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))letIn_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))letIn_eval;
    methods->m_free = (void (*)(struct Any*))letIn_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))letIn_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))letIn_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))letIn_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))letIn_sizeOf;
    methods->m_structSize = letIn_structSize;
    return methods;
}

struct E_LetIn* letIn_new(struct D_List* bindings, int nBindings, struct Any* expr) {
    struct E_LetIn* self = (struct E_LetIn*)gc_alloc(T_LetIn);
    self->nBindings = nBindings;
    self->bindings = bindings;
    self->expr = expr;
    return self;
}

void letIn_free(struct E_LetIn* self) {
    free(self);
}

struct E_LetIn* letIn_deepCopy(struct E_LetIn* self) {
    return letIn_new(list_deepCopy(self->bindings), self->nBindings, any_deepCopy(self->expr));
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct E_LetIn* letIn = (struct E_LetIn*)arg;
    struct D_Triple* env = evaluator_getEnv(etor);
    struct D_List* bindings = letIn->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* lhs = binding_getKey(binding);
        struct Any* rhsValue = evaluator_popObj(etor);
        env = any_match(lhs, rhsValue, env);
        if (env == NULL) {
            evaluator_throwException(
                etor,
                symbol_new("LetIn"),
                "no match between lhs and rhs",
                (struct Any*)array_newN(2, lhs, rhsValue)
            );
        }
        bindings = (struct D_List*)list_getRest(bindings);
    }
    evaluator_saveEnv(etor);
    evaluator_pushExprEnv(etor, letIn->expr, (struct Any*)env);
}

void letIn_eval(struct E_LetIn* self, struct Evaluator* etor) {
    struct E_Continuation* contin = continuation_new(_contin, "letIn", (struct Any*)self);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct D_List* bindings = self->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        evaluator_pushExpr(etor, (struct Any*)binding_getValue(binding));
        bindings = (struct D_List*)list_getRest(bindings);
    }
}

void letIn_freeVars(struct E_LetIn* self, struct D_Set* freeVars, struct Evaluator* etor) {
    // TODO finish this
    (void)self;
    (void)freeVars;
    (void)etor;
    printf("%s is incomplete\n", __func__);
}

void letIn_markChildren(struct E_LetIn* self) {
    any_mark((struct Any*)self->bindings);
    any_mark(self->expr);
}

void letIn_show(struct E_LetIn* self, FILE* fp) {
    list_showWith(self->bindings, "let ", ". ", "", fp);
    fputs(" in ", fp);
    any_show(self->expr, fp);
}

size_t letIn_sizeOf(struct E_LetIn* self) {
    return sizeof(self);
}

size_t letIn_structSize(void) {
    return sizeof(struct E_LetIn);
}
