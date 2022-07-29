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
#include "expr/let.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_Let {
    struct Any obj;
    int nBindings;
    struct D_List* bindings;
};

struct E_Let* let_new(struct D_List* bindings, int nBindings) {
    struct E_Let* self = (struct E_Let*)gc_alloc(T_Let);
    self->nBindings = nBindings;
    self->bindings = bindings;
    return self;
}

void let_free(struct E_Let* self) {
    free(self);
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct E_Let* let = (struct E_Let*)arg;
    struct D_Triple* env = evaluator_getEnv(etor);
    struct D_List* bindings = let->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* lhs = binding_getKey(binding);
        struct Any* rhsValue = evaluator_popObj(etor);
        env = any_match(lhs, rhsValue, env);
        if (env == NULL) {
            evaluator_throwException(
                etor,
                symbol_new("Let"),
                "no match between lhs and rhs",
                (struct Any*)array_newN(2, lhs, rhsValue)
            );
        }
        bindings = (struct D_List*)list_getRest(bindings);
    }
    evaluator_setEnv(etor, env);
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

struct E_Let* let_deepCopy(struct E_Let* self) {
    return let_new(list_deepCopy(self->bindings), self->nBindings);
}

void let_eval(struct E_Let* self, struct Evaluator* etor) {
    struct E_Continuation* contin = continuation_new(_contin, "let", (struct Any*)self);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct D_List* bindings = self->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        evaluator_pushExpr(etor, (struct Any*)binding_getValue(binding));
        bindings = (struct D_List*)list_getRest(bindings);
    }
}

void let_freeVars(struct E_Let* self, struct D_Set* freeVars, struct Evaluator* etor) {
    struct D_List* bindings = self->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* rhs = binding_getValue(binding);
        any_freeVars(rhs, freeVars, etor);
    }    
}

void let_markChildren(struct E_Let* self) {
    any_mark((struct Any*)self->bindings);
}

void let_show(struct E_Let* self, FILE* fp) {
    list_showWith(self->bindings, "let ", ". ", "", fp);
}

size_t let_sizeOf(struct E_Let* self) {
    return sizeof(self);
}

size_t let_structSize() {
    return sizeof(struct E_Let);
}
