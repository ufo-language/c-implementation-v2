#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "expr/loop.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_Loop {
    struct Any obj;
    struct Any* iterExpr;
    struct Any* iterator;
    struct Any* body;
};

struct Methods* loop_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))loop_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))loop_eval;
    methods->m_free = (void (*)(struct Any*))loop_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))loop_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))loop_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))loop_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))loop_sizeOf;
    methods->m_structSize = loop_structSize;
    return methods;
}

struct E_Loop* loop_new(struct Any* iterExpr, struct Any* body) {
    struct E_Loop* self = (struct E_Loop*)gc_alloc(T_Loop);
    self->iterExpr = iterExpr;
    self->iterator = (struct Any*)NIL;
    self->body = body;
    return self;
}

void loop_free(struct E_Loop* self) {
    free(self);
}

#if 0
static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct E_Loop* loop = (struct E_Loop*)arg;
    struct D_Triple* env = evaluator_getEnv(etor);
    struct D_List* bindings = loop->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* lhs = binding_getKey(binding);
        struct Any* rhsValue = evaluator_popObj(etor);
        env = any_match(lhs, rhsValue, env);
        if (env == NULL) {
            evaluator_throwException(
                etor,
                symbol_new("Loop"),
                "no match between lhs and rhs",
                (struct Any*)array_newN(2, lhs, rhsValue)
            );
        }
        bindings = (struct D_List*)list_getRest(bindings);
    }
    evaluator_setEnv(etor, env);
    evaluator_pushObj(etor, (struct Any*)NIL);
}
#endif

struct E_Loop* loop_deepCopy(struct E_Loop* self) {
    return loop_new(any_deepCopy(self->iterExpr), any_deepCopy(self->body));
}

static void _contin2(struct Evaluator* etor, struct Any* arg) {
    printf("%s got here 1, arg = ", __func__); any_show((struct Any*)arg, stdout); printf("\n");
    // pop & discard previous value
    struct Any* res = evaluator_popObj(etor);
    (void)res;

    struct D_Array* argAry = (struct D_Array*)arg;
    struct E_Identifier* ident = (struct E_Identifier*)array_get_unsafe(argAry, 0);
    struct D_Iterator* iter = (struct D_Iterator*)array_get_unsafe(argAry, 1);
    struct Any* body = array_get_unsafe(argAry, 2);
    struct D_Triple* binding = (struct D_Triple*)array_get_unsafe(argAry, 3);
    printf("  ident = "); any_show((struct Any*)ident, stdout); printf("\n");
    printf("  iter = "); any_show((struct Any*)iter, stdout); printf("\n");
    printf("  body = "); any_show((struct Any*)body, stdout); printf("\n");
    printf("  binding = "); any_show((struct Any*)binding, stdout); printf("\n");
    if (iterator_hasNext(iter)) {
        
    }
    else {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
}

static void _contin1(struct Evaluator* etor, struct Any* arg) {
    printf("%s got here 1, arg = ", __func__); any_show((struct Any*)arg, stdout); printf("\n");
    struct D_Array* argAry = (struct D_Array*)arg;

    struct Any* identObj = array_get_unsafe(argAry, 0);
    struct D_Triple* binding = EMPTY_TRIPLE;
    if (T_Identifier == any_typeId(identObj)) {
        struct E_Identifier* ident = (struct E_Identifier*)identObj;
        binding = evaluator_bind(etor, ident, (struct Any*)NIL);
    }
    else {
        binding = EMPTY_TRIPLE;
    }
    struct Any* body = array_get_unsafe(argAry, 1);
    struct Any* iterExpr = evaluator_popObj(etor);

    printf("%s got here 2, iterExpr = ", __func__); any_show((struct Any*)iterExpr, stdout); printf("\n");
    // create the iterator from the iterator expression
    struct D_Iterator* iter = any_iterator(iterExpr);
    printf("%s got here 3, iterator = ", __func__); any_show((struct Any*)iter, stdout); printf("\n");

    struct D_Array* contin2arg = array_newN(4, identObj, iter, body, binding);
    struct E_Continuation* contin = continuation_new(_contin2, "loop", (struct Any*)contin2arg);
    evaluator_pushExpr(etor, (struct Any*)contin);
    // contin2 pops the previous value and discards it, so push a dummy value
    evaluator_pushObj(etor, (struct Any*)NIL);
}

void loop_eval(struct E_Loop* self, struct Evaluator* etor) {
    printf("%s got here 1, self = ", __func__); any_show((struct Any*)self, stdout); printf("\n");
    struct Any* iterExpr = self->iterExpr;
    struct Any* identObj = (struct Any*)NIL;
    if (T_Binding == any_typeId(iterExpr)) {
        struct D_Binding* binding = (struct D_Binding*)iterExpr;
        identObj = binding_getKey(binding);
        iterExpr = binding_getValue(binding);
    }
    struct D_Array* contin1Arg = array_newN(2, identObj, self->body);
    struct E_Continuation* contin = continuation_new(_contin1, "loop", (struct Any*)contin1Arg);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, iterExpr);
}

void loop_freeVars(struct E_Loop* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->iterExpr, freeVars, etor);
    any_freeVars(self->body, freeVars, etor);
}

void loop_markChildren(struct E_Loop* self) {
    any_mark((struct Any*)self->iterExpr);
    any_mark((struct Any*)self->iterator);
    any_mark((struct Any*)self->body);
}

void loop_show(struct E_Loop* self, FILE* fp) {
    fputs("loop ", fp);
    any_show(self->iterExpr, fp);
    fputc(' ', fp);
    any_show(self->body, fp);
}

size_t loop_sizeOf(struct E_Loop* self) {
    return sizeof(self);
}

size_t loop_structSize(void) {
    return sizeof(struct E_Loop);
}
