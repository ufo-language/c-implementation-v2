#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/set.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/letrec.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct E_LetRec {
    struct Any obj;
    int nBindings;
    struct D_List* bindings;
};

struct Methods* letRec_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))letRec_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))letRec_eval;
    methods->m_free = (void (*)(struct Any*))letRec_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))letRec_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))letRec_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))letRec_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))letRec_sizeOf;
    methods->m_structSize = letRec_structSize;
    return methods;
}

struct E_LetRec* letRec_new(struct D_List* bindings, int nBindings) {
    struct E_LetRec* self = (struct E_LetRec*)gc_alloc(T_LetRec);
    self->nBindings = nBindings;
    self->bindings = bindings;
    return self;
}

void letRec_free(struct E_LetRec* self) {
    free(self);
}

static void _contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct Any* arg = continuation_getArg(contin);
    struct D_Queue* bindingListQueue = (struct D_Queue*)arg;
    while (!queue_isEmpty(bindingListQueue)) {
        struct D_Triple* binding = (struct D_Triple*)queue_deq(bindingListQueue, etor);
        struct Any* value = evaluator_popObj(etor);
        triple_setSecond(binding, value);
    }
}

struct E_LetRec* letRec_deepCopy(struct E_LetRec* self) {
    return letRec_new(list_deepCopy(self->bindings), self->nBindings);
}

void letRec_eval(struct E_LetRec* self, struct Evaluator* etor) {
    // Create an empty queue of RHS values so that it can be attached to the continuation,
    // so that the continuation can be pushed first.
    struct D_Queue* bindingListQueue = queue_new();
    struct E_Continuation* contin = continuation_new(_contin, "letRec", (struct Any*)bindingListQueue);
    // all expressions must use the dynamic environment
    evaluator_pushExprEnv(etor, (struct Any*)contin, (struct Any*)NIL);

    // Collect all the free variables, and at the same time push each
    // RHS to be evaluated.
    // TODO write the letRec_freeVars function below, and then use that function
    int nBindings = self->nBindings;
    struct D_List* bindings = self->bindings;
    struct D_Array* lhsAry = array_new(nBindings);
    for (int n=0; n<nBindings; n++) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* lhs = binding_getKey(binding);
        array_set_unsafe(lhsAry, n, lhs);
        struct Any* rhs = binding_getValue(binding);
        evaluator_pushExprEnv(etor, rhs, (struct Any*)NIL);
        bindings = (struct D_List*)list_getRest(bindings);
    }
    struct D_Set* freeVarSet = set_new();
    array_freeVars(lhsAry, freeVarSet, etor);

    // Bind all the free variables to nil, This is done so that
    // the identifiers are all in the environment before the RHSs are
    // evaluated.
    // The use of the dynamic environment ensures that when the RHSs
    // are evaluated, they are evaluated in the environment in which
    // we just bound all the identifiers to nil.
    struct D_Array* freeVarAry = set_toArray(freeVarSet);
    for (int n=0; n<array_count(freeVarAry); n++) {
        struct E_Identifier* ident = (struct E_Identifier*)array_get_unsafe(freeVarAry, n);
        struct D_Triple* binding = evaluator_bind(etor, ident, (struct Any*)NIL);
        queue_enq(bindingListQueue, (struct Any*)binding);
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}

void letRec_freeVars(struct E_LetRec* self, struct D_Set* freeVars, struct Evaluator* etor) {
    // TODO finish this
    (void)self;
    (void)freeVars;
    (void)etor;
    printf("%s is not implemented\n", __func__);
}

void letRec_markChildren(struct E_LetRec* self) {
    any_mark((struct Any*)self->bindings);
}

void letRec_show(struct E_LetRec* self, FILE* fp) {
    list_showWith(self->bindings, "letrec ", ", ", "", fp);
}

size_t letRec_sizeOf(struct E_LetRec* self) {
    return sizeof(self);
}

size_t letRec_structSize(void) {
    return sizeof(struct E_LetRec);
}
